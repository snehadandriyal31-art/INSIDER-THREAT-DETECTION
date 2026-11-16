// activity_manager.cpp — C++ manager calling C list functions through extern "C". Owner: M3.
#include "activity_manager.h"
#include "util.h"
#include <cmath>
#include <cstring>

ActivityManager::ActivityManager() : log_(list_create()) {}
ActivityManager::~ActivityManager() { list_free(log_); }          // RAII: C memory freed by C

void ActivityManager::logEvent(const Event& e) { list_append(log_, &e); }

// Callback handed to the C traversal; ctx is our std::vector
static void collect_cb(const Event* e, void* ctx) {
    static_cast<std::vector<Event>*>(ctx)->push_back(*e);
}

std::vector<Event> ActivityManager::allEvents() const {
    std::vector<Event> v;
    v.reserve(list_size(log_));
    list_traverse(log_, collect_cb, &v);
    return v;
}

std::vector<Event> ActivityManager::getHistory(const std::string& empId) const {
    return searchActivity(empId);
}

std::vector<Event> ActivityManager::searchActivity(const std::string& empId) const {
    std::vector<Event> out;
    for (ListNode* n = log_->head; n; n = n->next)
        if (empId == n->data.emp_id) out.push_back(n->data);
    return out;
}

std::vector<Event> ActivityManager::searchActivity(EventType type) const {
    std::vector<Event> out;
    for (ListNode* n = log_->head; n; n = n->next)
        if (n->data.type == type) out.push_back(n->data);
    return out;
}

std::vector<Event> ActivityManager::searchActivity(long day) const {
    std::vector<Event> out;
    for (ListNode* n = log_->head; n; n = n->next)
        if (util_ts_day(n->data.ts) == day) out.push_back(n->data);
    return out;
}

long ActivityManager::latestDay() const {
    long best = -1;
    for (ListNode* n = log_->head; n; n = n->next) {
        long d = util_ts_day(n->data.ts);
        if (d > best) best = d;
    }
    return best;
}

DayStats ActivityManager::dayStats(const std::string& empId, long day) const {
    DayStats s{};
    for (ListNode* n = log_->head; n; n = n->next) {
        const Event& e = n->data;
        if (empId != e.emp_id || util_ts_day(e.ts) != day) continue;
        s.total_events++;
        if (e.type == EV_FILE_DOWNLOAD) s.downloads++;
        if (e.type == EV_LOGIN_FAIL)    s.failed_logins++;
        if (e.type == EV_USB_INSERT)    s.usb_events++;
        if (util_is_after_hours(e.ts))  s.after_hours++;
    }
    return s;
}

static void mean_std(const std::vector<double>& xs, double& mean, double& sd) {
    mean = 0; sd = 0;
    if (xs.empty()) return;
    for (double x : xs) mean += x;
    mean /= xs.size();
    double var = 0;
    for (double x : xs) var += (x - mean) * (x - mean);
    var /= xs.size();                        // population std-dev
    sd = std::sqrt(var);
}

Baseline ActivityManager::computeBaseline(const std::string& empId, long today, int window) const {
    std::vector<double> ev, dl, ah, fl;
    for (long d = today - window; d < today; d++) {   // the `window` days BEFORE today
        DayStats s = dayStats(empId, d);
        ev.push_back(s.total_events);
        dl.push_back(s.downloads);
        ah.push_back(s.after_hours);
        fl.push_back(s.failed_logins);
    }
    Baseline b{};
    std::strncpy(b.emp_id, empId.c_str(), ID_LEN - 1);
    b.days = window;
    mean_std(ev, b.ev_mean, b.ev_std);
    mean_std(dl, b.dl_mean, b.dl_std);
    mean_std(ah, b.ah_mean, b.ah_std);
    mean_std(fl, b.fl_mean, b.fl_std);
    return b;
}

void ActivityManager::rebuildBaselines(const std::vector<std::string>& empIds, long today,
                                       int window, HashTable* ht) const {
    for (const std::string& id : empIds) {
        Baseline b = computeBaseline(id, today, window);
        ht_put(ht, &b);                                 // C hash table, O(1) insert
    }
}
