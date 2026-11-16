// alert_manager.h — owns the C queue (review line) + C stack (undo). Owner: M3.
#ifndef ALERT_MANAGER_H
#define ALERT_MANAGER_H
#include <string>
#include <vector>
#include "types.h"
#include "score.h"
#include "queue.h"
#include "stack.h"
#include "hashtable.h"
#include "employee.h"

class AlertManager {
public:
    AlertManager();                                  // queue_create(), stack_create()
    ~AlertManager();                                 // queue_free(),   stack_free()
    AlertManager(const AlertManager&) = delete;
    AlertManager& operator=(const AlertManager&) = delete;

    // Score one employee for `today` using the C scoring engine; stores result in Employee
    RiskReport scoreEmployee(Employee& e, const DayStats& today, const HashTable* baselines);

    // HIGH band -> enqueue for human review (no duplicates). Returns true if enqueued.
    bool raiseAlert(const Employee& e, const RiskReport& r, long now);
    bool restoreAlert(const Alert& a);               // used by persistence on startup

    std::vector<Alert> pending() const;              // snapshot of the queue (FIFO order)
    bool processNext(AnalystAction action, const std::string& analystId, Alert* out);
    bool undoLast(ActionRecord* out);                // pop stack, put alert back at queue front
    const ActionRecord* lastAction() const { return stack_peek(undo_); }

    static void rankByRisk(std::vector<Alert>& alerts);   // hand-written quicksort

    size_t pendingCount() const { return queue_size(queue_); }
    size_t undoDepth() const    { return stack_size(undo_); }

private:
    Queue* queue_;
    Stack* undo_;
};

#endif
