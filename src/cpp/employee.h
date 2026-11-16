// employee.h — Employee : User (inheritance). Owner: M3.
#ifndef EMPLOYEE_H
#define EMPLOYEE_H
#include "user.h"
#include "types.h"
#include "score.h"

class Employee : public User {
public:
    Employee(const std::string& id, const std::string& name, const std::string& dept);

    std::string role() const override { return "Employee"; }
    void generateSummary(std::ostream& os) const override;

    void setBaseline(const Baseline& b)      { baseline_ = b; hasBaseline_ = true; }
    bool hasBaseline() const                 { return hasBaseline_; }
    const Baseline& getBaseline() const      { return baseline_; }

    void setTodayStats(const DayStats& s)    { today_ = s; }
    const DayStats& getTodayStats() const    { return today_; }

    void setRisk(const RiskReport& r)        { risk_ = r; scored_ = true; }
    bool isScored() const                    { return scored_; }
    const RiskReport& getRisk() const        { return risk_; }
    double getRiskScore() const              { return scored_ ? risk_.score : 0.0; }

private:
    Baseline   baseline_{};
    DayStats   today_{};
    RiskReport risk_{};
    bool       hasBaseline_ = false;
    bool       scored_      = false;
};

#endif
