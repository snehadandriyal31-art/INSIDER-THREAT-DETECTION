// alert_manager.cpp — scoring + alert workflow on top of C queue/stack/sort/search. Owner: M3.
#include "alert_manager.h"
#include "sort.h"
#include "search.h"
#include "util.h"
#include <cstdio>
#include <cstring>

AlertManager::AlertManager() : queue_(queue_create()), undo_(stack_create()) {}
AlertManager::~AlertManager() { queue_free(queue_); stack_free(undo_); }   // RAII

RiskReport AlertManager::scoreEmployee(Employee& e, const DayStats& today, const HashTable* baselines) {
    RiskReport r{};
    const Baseline* b = ht_get(baselines, e.getId().c_str());   // O(1) hash lookup
    if (!b) return r;                                            // no baseline -> score 0
    e.setBaseline(*b);
    e.setTodayStats(today);
    r = compute_risk(&today, b);                                 // C scoring engine
    e.setRisk(r);
    return r;
}

bool AlertManager::raiseAlert(const Employee& e, const RiskReport& r, long now) {
    if (r.band != BAND_HIGH) return false;
    // avoid a duplicate alert for the same employee
    std::vector<Alert> cur = pending();
    if (linear_search_alert(cur.data(), (int)cur.size(), e.getId().c_str()) >= 0) return false;

    Alert a{};
    std::strncpy(a.emp_id, e.getId().c_str(), ID_LEN - 1);
    a.score = r.score;
    a.band = r.band;
    a.raised_at = now;
    char flags[96];
    explain_flags(r.flags, flags, sizeof flags);
    std::snprintf(a.reason, REASON_LEN, "z: ev=%.1f dl=%.1f ah=%.1f fl=%.1f / rules: %s",
                  r.z_events, r.z_downloads, r.z_after_hours, r.z_failed, flags);
    return enqueue(queue_, &a) == 1;
}

bool AlertManager::restoreAlert(const Alert& a) { return enqueue(queue_, &a) == 1; }

std::vector<Alert> AlertManager::pending() const {
    std::vector<Alert> v(queue_size(queue_));
    size_t n = queue_to_array(queue_, v.data(), v.size());
    v.resize(n);
    return v;
}

bool AlertManager::processNext(AnalystAction action, const std::string& analystId, Alert* out) {
    Alert a;
    if (!dequeue(queue_, &a)) return false;           // FIFO: oldest alert first
    ActionRecord rec{};
    rec.alert = a;
    rec.action = action;
    std::strncpy(rec.analyst_id, analystId.c_str(), ID_LEN - 1);
    stack_push(undo_, &rec);                          // remember for undo (LIFO)
    if (out) *out = a;
    return true;
}

bool AlertManager::undoLast(ActionRecord* out) {
    ActionRecord rec;
    if (!stack_pop(undo_, &rec)) return false;
    enqueue_front(queue_, &rec.alert);                // alert returns to the head of the line
    if (out) *out = rec;
    return true;
}

void AlertManager::rankByRisk(std::vector<Alert>& alerts) {
    if (alerts.size() > 1)
        quicksort_by_risk(alerts.data(), 0, (int)alerts.size() - 1);
}
