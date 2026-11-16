// main.cpp — entry point: build managers -> loadAll -> menu loop -> saveAll -> cleanup. Owner: M1.
#include "app_state.h"
#include "persistence.h"
#include "ui.h"
#include <iostream>

// ---- AppState helpers (registry = C DynArray of C++ User*) ----
Employee* AppState::findEmployee(const std::string& id) const {
    for (size_t i = 0; i < arr_size(users); i++) {
        User* u = static_cast<User*>(arr_get(users, i));
        if (u->getId() == id) return dynamic_cast<Employee*>(u);   // nullptr if it is an Analyst
    }
    return nullptr;
}

Analyst* AppState::firstAnalyst() const {
    for (size_t i = 0; i < arr_size(users); i++)
        if (Analyst* a = dynamic_cast<Analyst*>(static_cast<User*>(arr_get(users, i)))) return a;
    return nullptr;
}

std::vector<Employee*> AppState::employees() const {
    std::vector<Employee*> v;
    for (size_t i = 0; i < arr_size(users); i++)
        if (Employee* e = dynamic_cast<Employee*>(static_cast<User*>(arr_get(users, i)))) v.push_back(e);
    return v;
}

std::vector<std::string> AppState::employeeIds() const {
    std::vector<std::string> v;
    for (Employee* e : employees()) v.push_back(e->getId());
    return v;
}

int main(int argc, char** argv) {
    AppState st;
    st.dataDir   = argc > 1 ? argv[1] : "data";
    st.users     = arr_create(16);          // C
    st.baselines = ht_create(64);           // C
    st.activity  = new ActivityManager();   // C++ (owns a C linked list)
    st.alerts    = new AlertManager();      // C++ (owns a C queue + C stack)

    std::cout << "Loading data from " << st.dataDir << "/ ...\n";
    if (!persistence::loadAll(st)) {
        std::cerr << "Failed to load data files. Run from the project root (or pass the data dir).\n";
    } else {
        std::cout << "Loaded " << arr_size(st.users) << " users, " << st.activity->size()
                  << " events, " << ht_count(st.baselines) << " baselines, "
                  << st.alerts->pendingCount() << " pending alerts.\n";
        ConsoleUI ui(st);
        ui.run();
    }

    // ---- cleanup: strict ownership rule ----
    for (size_t i = 0; i < arr_size(st.users); i++)
        delete static_cast<User*>(arr_get(st.users, i));   // new'd in C++  -> delete (virtual dtor)
    arr_free(st.users);                                     // malloc'd in C -> C free function
    ht_free(st.baselines);
    delete st.activity;                                     // destructor calls list_free()
    delete st.alerts;                                       // destructor calls queue_free()/stack_free()
    return 0;
}
