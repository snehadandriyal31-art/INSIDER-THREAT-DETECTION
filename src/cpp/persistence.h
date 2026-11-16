// persistence.h — save/load the 4 text files in data/. Owner: M3.
#ifndef PERSISTENCE_H
#define PERSISTENCE_H
#include "app_state.h"

namespace persistence {
    bool loadAll(AppState& st);    // employees, activity, baselines, alerts
    bool saveAll(const AppState& st);
    bool saveActivity(const AppState& st);
    bool saveBaselines(const AppState& st);
    bool saveAlerts(const AppState& st);
}

#endif
