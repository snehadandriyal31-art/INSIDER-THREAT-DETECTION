// ui.cpp — console menu. Each option shows one part of the pipeline. Owner: M3.
#include "ui.h"
#include "persistence.h"
#include "util.h"
#include "sort.h"
#include "search.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using std::cout;
using std::cin;

void ConsoleUI::showMenu() const {
    char day[16] = "n/a";
    long today = st_.activity->latestDay();
    if (today >= 0) util_format_day(today, day, sizeof day);
    cout << "\n==========================================================\n"
         << "  INSIDER THREAT DETECTION  —  behavioural analysis console\n"
         << "  log: " << st_.activity->size() << " events | today: " << day
         << " | pending alerts: " << st_.alerts->pendingCount()
         << " | undo depth: " << st_.alerts->undoDepth() << "\n"
         << "==========================================================\n"
         << "  1. Display users               (OOP: inheritance + polymorphism)\n"
         << "  2. View activity log           (C linked list)\n"
         << "  3. Employee summary            (hash-table baseline lookup)\n"
         << "  4. Add simulated activity      (list_append)\n"
         << "  5. Sort + search events        (quicksort + binary search)\n"
         << "  6. Compute risk scores today   (z-score + rules -> 0-100 -> band)\n"
         << "  7. View alert queue            (FIFO queue)\n"
         << "  8. Process next alert          (dequeue + push undo stack)\n"
         << "  9. Undo last analyst action    (stack pop)\n"
         << " 10. Risk-ranked view            (quicksort by score)\n"
         << " 11. Rebuild 30-day baselines    (statistics over history)\n"
         << " 12. Save & exit\n";
}

int ConsoleUI::readInt(const char* prompt, int lo, int hi) {
    std::string line;
    while (true) {
        cout << prompt;
        if (!std::getline(cin, line)) { eof_ = true; return lo; }
        try {
            size_t pos = 0;
            int v = std::stoi(line, &pos);
            if (pos == line.size() && v >= lo && v <= hi) return v;
        } catch (...) {}
        cout << "  ! enter a number between " << lo << " and " << hi << "\n";
    }
}

std::string ConsoleUI::readString(const char* prompt) {
    std::string line;
    while (true) {
        cout << prompt;
        if (!std::getline(cin, line)) { eof_ = true; return ""; }
        if (!line.empty() && line.find('|') == std::string::npos) return line;
        cout << "  ! input must be non-empty and must not contain '|'\n";
    }
}

void ConsoleUI::run() {
    while (!eof_) {
        showMenu();
        int c = readInt("Choose option: ", 1, 12);
        if (eof_) break;
        switch (c) {
            case 1:  displayUsers();       break;
            case 2:  viewActivityLog();    break;
            case 3:  employeeSummary();    break;
            case 4:  addActivity();        break;
            case 5:  sortAndSearch();      break;
            case 6:  computeRiskScores();  break;
            case 7:  viewAlertQueue();     break;
            case 8:  processNextAlert();   break;
            case 9:  undoLastAction();     break;
            case 10: riskRankedView();     break;
            case 11: rebuildBaselines();   break;
            case 12:
                cout << (persistence::saveAll(st_) ? "Saved data/. Goodbye.\n" : "! save failed\n");
                return;
        }
    }
    cout << "\n(input ended) saving...\n";
    persistence::saveAll(st_);
}

// ---------- 1 ----------
void ConsoleUI::displayUsers() {
    cout << "\n  ID    NAME            DEPARTMENT    ROLE\n";
    for (size_t i = 0; i < arr_size(st_.users); i++) {
        User* u = static_cast<User*>(arr_get(st_.users, i));   // C array -> C++ object
        u->displayLine(cout);                                   // virtual role() dispatch
    }
    cout << "  (" << arr_size(st_.users) << " users; same loop, different role() — polymorphism)\n";
}

// ---------- 2 ----------
void ConsoleUI::viewActivityLog() {
    long today = st_.activity->latestDay();
    int mode = readInt("  [1] today's events  [2] last 25 of whole log  [3] one employee: ", 1, 3);
    std::vector<Event> ev;
    if (mode == 1)      ev = st_.activity->searchActivity(today);                    // overload (long)
    else if (mode == 3) ev = st_.activity->searchActivity(readString("  employee id: ")); // overload (string)
    else {
        std::vector<Event> all = st_.activity->allEvents();
        size_t from = all.size() > 25 ? all.size() - 25 : 0;
        ev.assign(all.begin() + from, all.end());
    }
    char ts[32];
    cout << "\n  TIME              EMP    EVENT\n";
    for (const Event& e : ev) {
        util_format_ts(e.ts, ts, sizeof ts);
        cout << "  " << ts << "  " << std::left << std::setw(6) << e.emp_id
             << " " << util_event_name(e.type) << (util_is_after_hours(e.ts) ? "   <- after hours" : "") << "\n";
    }
    cout << "  (" << ev.size() << " events shown; total in linked list: " << st_.activity->size() << ")\n";
}

// ---------- 3 ----------
void ConsoleUI::employeeSummary() {
    std::string id = readString("  employee id (e.g. E104): ");
    Employee* e = st_.findEmployee(id);
    if (!e) { cout << "  ! no employee " << id << "\n"; return; }
    long today = st_.activity->latestDay();
    const Baseline* b = ht_get(st_.baselines, id.c_str());      // O(1) lookup in C hash table
    if (b) e->setBaseline(*b);
    e->setTodayStats(st_.activity->dayStats(id, today));
    e->generateSummary(cout);
}

// ---------- 4 ----------
void ConsoleUI::addActivity() {
    std::string id = readString("  employee id: ");
    if (!st_.findEmployee(id)) { cout << "  ! unknown employee\n"; return; }
    cout << "  event types: ";
    for (int i = 0; i < EV_COUNT; i++) cout << i + 1 << "=" << util_event_name((EventType)i) << " ";
    cout << "\n";
    int t = readInt("  type number: ", 1, EV_COUNT);
    int hh = readInt("  hour (0-23) today: ", 0, 23);
    int mm = readInt("  minute (0-59): ", 0, 59);
    Event e{};
    std::strncpy(e.emp_id, id.c_str(), ID_LEN - 1);
    e.type = (EventType)(t - 1);
    e.ts = st_.activity->latestDay() * 1440L + hh * 60L + mm;
    st_.activity->logEvent(e);
    char ts[32]; util_format_ts(e.ts, ts, sizeof ts);
    cout << "  appended to linked list: " << ts << " " << id << " " << util_event_name(e.type)
         << (util_is_after_hours(e.ts) ? " (after hours!)" : "") << "\n";
}

// ---------- 5 ----------
void ConsoleUI::sortAndSearch() {
    long today = st_.activity->latestDay();
    std::vector<Event> ev = st_.activity->searchActivity(today);
    if (ev.empty()) { cout << "  no events today\n"; return; }
    quicksort_events_by_time(ev.data(), 0, (int)ev.size() - 1);  // C quicksort on a C++ vector buffer
    char ts[32];
    cout << "  sorted " << ev.size() << " of today's events by time (quicksort). First 5:\n";
    for (size_t i = 0; i < ev.size() && i < 5; i++) {
        util_format_ts(ev[i].ts, ts, sizeof ts);
        cout << "    " << ts << " " << ev[i].emp_id << " " << util_event_name(ev[i].type) << "\n";
    }
    int hh = readInt("  search time — hour (0-23): ", 0, 23);
    int mm = readInt("  minute (0-59): ", 0, 59);
    int idx = binary_search_by_time(ev.data(), (int)ev.size(), today * 1440L + hh * 60L + mm);
    if (idx < 0) cout << "  binary search: no event at that exact minute\n";
    else {
        util_format_ts(ev[idx].ts, ts, sizeof ts);
        cout << "  binary search hit at index " << idx << ": " << ts << " " << ev[idx].emp_id
             << " " << util_event_name(ev[idx].type) << "\n";
    }
    std::string id = readString("  linear search — employee id: ");
    int li = linear_search_event(ev.data(), (int)ev.size(), id.c_str());
    cout << (li < 0 ? "  linear search: no event for that employee today\n"
                    : "  linear search: first event at index " + std::to_string(li) + "\n");
}

// ---------- 6 ----------
void ConsoleUI::computeRiskScores() {
    long today = st_.activity->latestDay();
    char day[16]; util_format_day(today, day, sizeof day);
    long now = today * 1440L + 23 * 60L + 59;
    cout << "\n  Risk scores for " << day << "  (baseline = previous 30 days)\n";
    cout << "  EMP    EVENTS DL  AH  FAIL USB | z-ev  z-dl  z-ah  z-fl | RULES                     | SCORE  BAND\n";
    cout << std::fixed << std::setprecision(1);
    int raised = 0;
    for (Employee* e : st_.employees()) {
        DayStats s = st_.activity->dayStats(e->getId(), today);
        RiskReport r = st_.alerts->scoreEmployee(*e, s, st_.baselines);
        if (!e->hasBaseline()) { cout << "  " << e->getId() << "  (no baseline — run option 11)\n"; continue; }
        char flags[96]; explain_flags(r.flags, flags, sizeof flags);
        cout << "  " << std::left << std::setw(6) << e->getId()
             << " " << std::setw(6) << s.total_events << std::setw(4) << s.downloads << std::setw(4) << s.after_hours
             << std::setw(5) << s.failed_logins << std::setw(4) << s.usb_events
             << "| " << std::setw(5) << r.z_events << " " << std::setw(5) << r.z_downloads << " "
             << std::setw(5) << r.z_after_hours << " " << std::setw(5) << r.z_failed
             << "| " << std::setw(26) << flags << "| " << std::setw(6) << r.score << " " << util_band_name(r.band) << "\n";
        if (st_.alerts->raiseAlert(*e, r, now)) raised++;
    }
    cout << "  -> " << raised << " new HIGH-risk alert(s) auto-enqueued for analyst review (option 7).\n"
         << "  Note: a high score is a risk indicator, not proof of misconduct.\n";
}

// ---------- 7 ----------
void ConsoleUI::viewAlertQueue() {
    std::vector<Alert> q = st_.alerts->pending();
    if (q.empty()) { cout << "  alert queue is empty (run option 6 first)\n"; return; }
    char ts[32];
    cout << "\n  # EMP    SCORE  BAND  RAISED            REASON\n";
    for (size_t i = 0; i < q.size(); i++) {
        util_format_ts(q[i].raised_at, ts, sizeof ts);
        cout << "  " << i + 1 << " " << std::left << std::setw(6) << q[i].emp_id
             << std::fixed << std::setprecision(1) << std::setw(6) << q[i].score << " "
             << std::setw(5) << util_band_name(q[i].band) << " " << ts << "  " << q[i].reason << "\n";
    }
    cout << "  (FIFO: #1 is reviewed first)\n";
}

// ---------- 8 ----------
void ConsoleUI::processNextAlert() {
    if (st_.alerts->pendingCount() == 0) { cout << "  nothing to review\n"; return; }
    Analyst* an = st_.firstAnalyst();
    std::string aid = an ? an->getId() : "A000";
    std::vector<Alert> q = st_.alerts->pending();
    cout << "  next in queue: " << q[0].emp_id << " score " << std::fixed << std::setprecision(1) << q[0].score
         << " — " << q[0].reason << "\n";
    int c = readInt("  action [1] reviewed  [2] escalated  [3] dismissed: ", 1, 3);
    AnalystAction act = c == 1 ? ACT_REVIEWED : c == 2 ? ACT_ESCALATED : ACT_DISMISSED;
    if (act == ACT_ESCALATED && an && !an->canEscalate()) {
        cout << "  ! analyst " << aid << " lacks clearance to escalate; recorded as REVIEWED\n";
        act = ACT_REVIEWED;
    }
    Alert done;
    st_.alerts->processNext(act, aid, &done);
    cout << "  dequeued " << done.emp_id << " -> " << util_action_name(act) << " by " << aid
         << " (pushed on undo stack, depth " << st_.alerts->undoDepth() << ")\n";
}

// ---------- 9 ----------
void ConsoleUI::undoLastAction() {
    ActionRecord rec;
    if (!st_.alerts->undoLast(&rec)) { cout << "  undo stack is empty\n"; return; }
    cout << "  undone: " << util_action_name(rec.action) << " on " << rec.alert.emp_id
         << " by " << rec.analyst_id << " — alert restored to FRONT of queue\n";
}

// ---------- 10 ----------
void ConsoleUI::riskRankedView() {
    std::vector<Alert> rows;
    for (Employee* e : st_.employees()) {
        if (!e->isScored()) continue;
        Alert a{};
        std::strncpy(a.emp_id, e->getId().c_str(), ID_LEN - 1);
        a.score = e->getRiskScore();
        a.band = e->getRisk().band;
        rows.push_back(a);
    }
    if (rows.empty()) { cout << "  no scored employees yet (run option 6)\n"; return; }
    AlertManager::rankByRisk(rows);
    cout << "\n  RANK EMP    SCORE  BAND\n";
    for (size_t i = 0; i < rows.size(); i++)
        cout << "  " << std::left << std::setw(5) << i + 1 << std::setw(6) << rows[i].emp_id
             << std::fixed << std::setprecision(1) << std::setw(6) << rows[i].score << " " << util_band_name(rows[i].band) << "\n";
    cout << "  (hand-written quicksort, descending by score)\n";
}

// ---------- 11 ----------
void ConsoleUI::rebuildBaselines() {
    long today = st_.activity->latestDay();
    st_.activity->rebuildBaselines(st_.employeeIds(), today, 30, st_.baselines);
    cout << "  rebuilt baselines for " << ht_count(st_.baselines) << " employees from the 30 days before today\n";
    cout << std::fixed << std::setprecision(2) << "  EMP    EV/DAY        DL/DAY        AH/DAY        FAIL/DAY\n";
    for (const std::string& id : st_.employeeIds()) {
        const Baseline* b = ht_get(st_.baselines, id.c_str());
        if (!b) continue;
        cout << "  " << std::left << std::setw(6) << id
             << " " << std::setw(5) << b->ev_mean << "+/-" << std::setw(5) << b->ev_std
             << " " << std::setw(5) << b->dl_mean << "+/-" << std::setw(5) << b->dl_std
             << " " << std::setw(5) << b->ah_mean << "+/-" << std::setw(5) << b->ah_std
             << " " << std::setw(5) << b->fl_mean << "+/-" << std::setw(5) << b->fl_std << "\n";
    }
}
