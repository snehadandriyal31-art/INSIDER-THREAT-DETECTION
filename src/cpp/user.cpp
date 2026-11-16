// user.cpp — abstract User implementation. Owner: M3.
#include "user.h"
#include <iomanip>

User::User(const std::string& id, const std::string& name, const std::string& dept)
    : id_(id), name_(name), dept_(dept) {}

User::~User() {}

void User::displayLine(std::ostream& os) const {
    os << "  " << std::left << std::setw(6) << id_
       << std::setw(16) << name_
       << std::setw(14) << dept_
       << std::setw(10) << role() << "\n";
}
