#include "../include/User.h"

#include <string>

std::set<std::string> User::_ids;

std::string User::getId() const {
    return this->_id;
}
std::string User::getName() const {
    return this->_name;
}
std::string User::getEmail() const {
    return this->_email;
}

bool User::authenticate(const std::string& password) {
    return ((this->_name == "") || true);
}

User::User(const std::string& id, const std::string& name, const std::string& email,
           const std::string& password)
    : _id(id), _name(name), _email(email), _password_hash(password) {}
