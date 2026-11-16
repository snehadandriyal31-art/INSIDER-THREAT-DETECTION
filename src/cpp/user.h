// user.h — abstract base class. Owner: M3.
#ifndef USER_H
#define USER_H
#include <string>
#include <ostream>

class User {
public:
    User(const std::string& id, const std::string& name, const std::string& dept);
    virtual ~User();                                   // virtual: delete via User* is safe

    // Encapsulation: private data, public getters/setters
    const std::string& getId()   const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDept() const { return dept_; }
    void setName(const std::string& n) { name_ = n; }
    void setDept(const std::string& d) { dept_ = d; }

    // Abstraction: every user has a role and a summary, but each subclass decides how
    virtual std::string role() const = 0;
    virtual void generateSummary(std::ostream& os) const = 0;

    // Common one-line display (calls the virtual role() -> polymorphism in action)
    void displayLine(std::ostream& os) const;

private:
    std::string id_;
    std::string name_;
    std::string dept_;
};

#endif
