// activity_manager.h — owns the C linked-list activity log (RAII). Owner: M3.
#ifndef ACTIVITY_MANAGER_H
#define ACTIVITY_MANAGER_H
#include <string>
#include <vector>
#include "types.h"
#include "linkedlist.h"
#include "hashtable.h"

class ActivityManager {
public:
    ActivityManager();                                   // list_create()
    ~ActivityManager();                                  // list_free()  -> RAII
    ActivityManager(const ActivityManager&) = delete;    // one owner of the C list
    ActivityManager& operator=(const ActivityManager&) = delete;

    void logEvent(const Event& e);
    std::vector<Event> allEvents() const;                // traverse -> vector copy
    std::vector<Event> getHistory(const std::string& empId) const;

    // Function overloading: same name, different parameter types
    std::vector<Event> searchActivity(const std::string& empId) const;
    std::vector<Event> searchActivity(EventType type) const;
    std::vector<Event> searchActivity(long day) const;   // all events of one day

    long     latestDay() const;                          // "today" = newest day in log
    DayStats dayStats(const std::string& empId, long day) const;

    // Rolling baseline: mean/std of daily metrics over `window` days before `today`
    Baseline computeBaseline(const std::string& empId, long today, int window) const;
    void     rebuildBaselines(const std::vector<std::string>& empIds, long today,
                              int window, HashTable* ht) const;

    size_t size() const { return list_size(log_); }

private:
    LinkedList* log_;
};

#endif
