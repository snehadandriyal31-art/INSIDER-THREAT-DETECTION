// analyst.cpp — Analyst implementation. Owner: M3.
#include "analyst.h"

Analyst::Analyst(const std::string& id, const std::string& name, const std::string& dept, int clearance)
    : User(id, name, dept), clearance_(clearance) {}

void Analyst::generateSummary(std::ostream& os) const {
    os << "----------------------------------------------------------\n";
    os << " ANALYST " << getId() << "  " << getName() << "  (" << getDept() << ")\n";
    os << " Clearance level: " << clearance_
       << (canEscalate() ? "  — may review, dismiss and ESCALATE alerts\n"
                         : "  — may review and dismiss alerts\n");
}
