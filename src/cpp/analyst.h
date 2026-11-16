// analyst.h — Analyst : User (inheritance + polymorphism). Owner: M3.
#ifndef ANALYST_H
#define ANALYST_H
#include "user.h"

class Analyst : public User {
public:
    Analyst(const std::string& id, const std::string& name, const std::string& dept, int clearance = 1);

    std::string role() const override { return "Analyst"; }
    void generateSummary(std::ostream& os) const override;

    int  getClearance() const   { return clearance_; }
    bool canEscalate() const    { return clearance_ >= 2; }   // only senior analysts escalate

private:
    int clearance_;
};

#endif
