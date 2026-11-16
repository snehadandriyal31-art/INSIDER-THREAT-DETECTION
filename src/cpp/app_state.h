// app_state.h — everything main.cpp owns, passed to UI and persistence. Owner: M1.
#ifndef APP_STATE_H
#define APP_STATE_H
#include <string>
#include <vector>
#include "array.h"
#include "hashtable.h"
#include "activity_manager.h"
#include "alert_manager.h"
#include "employee.h"
#include "analyst.h"

struct AppState {
    DynArray*        users;        // C dynamic array holding C++ User* objects (the bridge!)
    HashTable*       baselines;    // C hash table: emp_id -> Baseline
    ActivityManager* activity;     // owns C linked list
    AlertManager*    alerts;       // owns C queue + C stack
    std::string      dataDir;

    // helpers over the registry
    Employee* findEmployee(const std::string& id) const;
    Analyst*  firstAnalyst() const;
    std::vector<Employee*> employees() const;
    std::vector<std::string> employeeIds() const;
};

#endif
