// ui.h — menu, validated input, one handler per option. Owner: M3.
#ifndef UI_H
#define UI_H
#include "app_state.h"

class ConsoleUI {
public:
    explicit ConsoleUI(AppState& st) : st_(st) {}
    void run();                                  // menu loop until "save & exit"

private:
    void showMenu() const;
    int         readInt(const char* prompt, int lo, int hi);
    std::string readString(const char* prompt);
    bool        eof() const { return eof_; }

    void displayUsers();          // 1
    void viewActivityLog();       // 2
    void employeeSummary();       // 3
    void addActivity();           // 4
    void sortAndSearch();         // 5
    void computeRiskScores();     // 6
    void viewAlertQueue();        // 7
    void processNextAlert();      // 8
    void undoLastAction();        // 9
    void riskRankedView();        // 10
    void rebuildBaselines();      // 11

    AppState& st_;
    bool eof_ = false;
};

#endif
