// persistence.cpp — pipe-delimited text files, loaded on startup / saved on exit. Owner: M3.
//
//   employees.txt : id|name|department|role[|clearance]
//   activity.txt  : emp_id|EVENT_TYPE|YYYY-MM-DD HH:MM
//   baselines.txt : emp_id|days|ev_mean|ev_std|dl_mean|dl_std|ah_mean|ah_std|fl_mean|fl_std
//   alerts.txt    : emp_id|score|BAND|YYYY-MM-DD HH:MM|reason
#include "persistence.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>

namespace {

std::vector<std::string> split(const std::string& line, char sep = '|') {
    std::vector<std::string> out;
    std::string cur;
    std::istringstream ss(line);
    while (std::getline(ss, cur, sep)) out.push_back(cur);
    return out;
}

void copyId(char* dst, const std::string& src) {
    std::strncpy(dst, src.c_str(), ID_LEN - 1);
    dst[ID_LEN - 1] = '\0';
}

bool loadEmployees(AppState& st) {
    std::ifstream in(st.dataDir + "/employees.txt");
    if (!in) { std::cerr << "[load] cannot open employees.txt\n"; return false; }
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto f = split(line);
        if (f.size() < 4) continue;
        User* u = nullptr;
        if (f[3] == "ANALYST") {
            int clr = f.size() >= 5 ? std::atoi(f[4].c_str()) : 1;
            u = new Analyst(f[0], f[1], f[2], clr);
        } else {
            u = new Employee(f[0], f[1], f[2]);
        }
        arr_push(st.users, u);                 // C++ object stored in C dynamic array
    }
    return true;
}

bool loadActivity(AppState& st) {
    std::ifstream in(st.dataDir + "/activity.txt");
    if (!in) { std::cerr << "[load] cannot open activity.txt\n"; return false; }
    std::string line;
    int bad = 0;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto f = split(line);
        Event e{};
        if (f.size() < 3 || !util_event_from_name(f[1].c_str(), &e.type) || !util_parse_ts(f[2].c_str(), &e.ts)) {
            bad++;
            continue;
        }
        copyId(e.emp_id, f[0]);
        st.activity->logEvent(e);
    }
    if (bad) std::cerr << "[load] skipped " << bad << " malformed activity lines\n";
    return true;
}

bool loadBaselines(AppState& st) {
    std::ifstream in(st.dataDir + "/baselines.txt");
    if (!in) return false;                      // optional: can be rebuilt from history
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto f = split(line);
        if (f.size() < 10) continue;
        Baseline b{};
        copyId(b.emp_id, f[0]);
        b.days    = std::atoi(f[1].c_str());
        b.ev_mean = std::atof(f[2].c_str()); b.ev_std = std::atof(f[3].c_str());
        b.dl_mean = std::atof(f[4].c_str()); b.dl_std = std::atof(f[5].c_str());
        b.ah_mean = std::atof(f[6].c_str()); b.ah_std = std::atof(f[7].c_str());
        b.fl_mean = std::atof(f[8].c_str()); b.fl_std = std::atof(f[9].c_str());
        ht_put(st.baselines, &b);
    }
    return true;
}

bool loadAlerts(AppState& st) {
    std::ifstream in(st.dataDir + "/alerts.txt");
    if (!in) return false;                      // optional
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto f = split(line);
        if (f.size() < 5) continue;
        Alert a{};
        copyId(a.emp_id, f[0]);
        a.score = std::atof(f[1].c_str());
        a.band  = f[2] == "HIGH" ? BAND_HIGH : f[2] == "MEDIUM" ? BAND_MEDIUM : BAND_LOW;
        util_parse_ts(f[3].c_str(), &a.raised_at);
        std::strncpy(a.reason, f[4].c_str(), REASON_LEN - 1);
        st.alerts->restoreAlert(a);
    }
    return true;
}

} // namespace

namespace persistence {

bool loadAll(AppState& st) {
    bool ok = loadEmployees(st) && loadActivity(st);
    bool hadBaselines = loadBaselines(st);
    loadAlerts(st);
    if (!hadBaselines) {                        // first run: derive baselines from history
        long today = st.activity->latestDay();
        st.activity->rebuildBaselines(st.employeeIds(), today, 30, st.baselines);
        std::cout << "[load] no baselines.txt — rebuilt from 30-day history\n";
    }
    return ok;
}

bool saveActivity(const AppState& st) {
    std::ofstream out(st.dataDir + "/activity.txt");
    if (!out) return false;
    out << "# emp_id|event_type|YYYY-MM-DD HH:MM\n";
    char ts[32];
    for (const Event& e : st.activity->allEvents()) {
        util_format_ts(e.ts, ts, sizeof ts);
        out << e.emp_id << '|' << util_event_name(e.type) << '|' << ts << '\n';
    }
    return true;
}

static void baseline_writer(const Baseline* b, void* ctx) {
    std::ostream& out = *static_cast<std::ostream*>(ctx);
    out << b->emp_id << '|' << b->days << '|'
        << b->ev_mean << '|' << b->ev_std << '|'
        << b->dl_mean << '|' << b->dl_std << '|'
        << b->ah_mean << '|' << b->ah_std << '|'
        << b->fl_mean << '|' << b->fl_std << '\n';
}

bool saveBaselines(const AppState& st) {
    std::ofstream out(st.dataDir + "/baselines.txt");
    if (!out) return false;
    out << std::fixed << std::setprecision(3);
    out << "# emp_id|days|ev_mean|ev_std|dl_mean|dl_std|ah_mean|ah_std|fl_mean|fl_std\n";
    ht_foreach(st.baselines, baseline_writer, &out);
    return true;
}

bool saveAlerts(const AppState& st) {
    std::ofstream out(st.dataDir + "/alerts.txt");
    if (!out) return false;
    out << std::fixed << std::setprecision(2);
    out << "# emp_id|score|band|raised_at|reason   (pending review queue, FIFO order)\n";
    char ts[32];
    for (const Alert& a : st.alerts->pending()) {
        util_format_ts(a.raised_at, ts, sizeof ts);
        out << a.emp_id << '|' << a.score << '|' << util_band_name(a.band) << '|' << ts << '|' << a.reason << '\n';
    }
    return true;
}

bool saveAll(const AppState& st) {
    return saveActivity(st) && saveBaselines(st) && saveAlerts(st);
}

} // namespace persistence
