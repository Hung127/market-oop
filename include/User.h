#ifndef USER_H_
#define USER_H_

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

class User {
   protected:
    std::string _id;
    std::string _name;
    std::string _email;
    std::string _password_hash;
    static std::set<std::string> _ids;

   protected:
    User(const std::string& id, const std::string& name, const std::string& email,
         const std::string& password);

   public:
    std::string getId() const;
    std::string getName() const;
    std::string getEmail() const;
    bool authenticate(const std::string& password);

   public:
    virtual ~User() = default;
};

#endif
