#ifndef USER_DTO_H
#define USER_DTO_H

#include <string>
#include "Utils.h"

class UserDto {
protected:
    std::string _id;
    std::string _name;
    std::string _email;
    std::string _password; // Lưu password thô hoặc đã hash tùy logic login

public:
    UserDto(const std::string& id, const std::string& name, const std::string& email, const std::string& password)
        : _id(id), _name(name), _email(email), _password(password) {}

    virtual ~UserDto() = default;

    std::string getId() const { return _id; }
    std::string getName() const { return _name; }
    std::string getEmail() const { return _email; }
    std::string getPassword() const { return _password; }

    bool authenticate(const std::string& inputPass) const {
        return _password == inputPass; 
    }
};

#endif
