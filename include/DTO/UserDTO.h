#ifndef USERDTO_H
#define USERDTO_H

#include <string>

#include "../Utils.h"

class UserDTO {
   protected:
    std::string _id;
    std::string _name;
    std::string _email;
    std::string _password;

   public:
    UserDTO(const std::string& id, const std::string& name, const std::string& email,
            const std::string& password)
        : _id(id), _name(name), _email(email), _password(password) {}

    virtual ~UserDTO() = default;

    std::string getId() const {
        return _id;
    }
    std::string getName() const {
        return _name;
    }
    std::string getEmail() const {
        return _email;
    }
    std::string getPassword() const {
        return _password;
    }
    virtual std::string getRole() const = 0;

    bool authenticate(const std::string& inputPass) const {
        return _password == inputPass;
    }
};

#endif
