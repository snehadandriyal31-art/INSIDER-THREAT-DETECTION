// employee.cpp — Employee summary: baseline vs today, plus explainable score. Owner: M3.
#include "employee.h"
#include "util.h"
#include <iomanip>

Employee::Employee(const std::string& id, const std::string& name, const std::string& dept)
    : User(id, name, dept) {}

void Employee::generateSummary(std::ostream& os) const {
    os << "----------------------------------------------------------\n";
    os << " EMPLOYEE " << getId() << "  " << getName() << "  (" << getDept() << ")\n";
    os << "----------------------------------------------------------\n";
    os << std::fixed << std::setprecision(2);
    if (hasBaseline_) {
        os << " Baseline (" << baseline_.days << "-day avg +/- std):\n";
        os << "   events/day      " << baseline_.ev_mean << " +/- " << baseline_.ev_std << "\n";
        os << "   downloads/day   " << baseline_.dl_mean << " +/- " << baseline_.dl_std << "\n";
        os << "   after-hours/day " << baseline_.ah_mean << " +/- " << baseline_.ah_std << "\n";
        os << "   failed logins   " << baseline_.fl_mean << " +/- " << baseline_.fl_std << "\n";
    } else {
        os << " Baseline: (none — rebuild baselines from history)\n";
    }
    os << " Today: events=" << today_.total_events
       << " downloads=" << today_.downloads
       << " after-hours=" << today_.after_hours
       << " failed-logins=" << today_.failed_logins
       << " usb=" << today_.usb_events << "\n";
    if (scored_) {
        char flags[96];
        explain_flags(risk_.flags, flags, sizeof flags);
        os << " z-scores: events=" << risk_.z_events
           << " downloads=" << risk_.z_downloads
           << " after-hours=" << risk_.z_after_hours
           << " failed=" << risk_.z_failed << "\n";
        os << " Score = z-part " << risk_.z_component
           << " + rules " << risk_.rule_component << " [" << flags << "]"
           << " = " << risk_.score << "  ->  " << util_band_name(risk_.band) << "\n";
    } else {
        os << " Risk: not scored yet (menu option 6)\n";
    }
}
