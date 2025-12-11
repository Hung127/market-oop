#include "../../include/DTO/UserDTO.h"

UserDTO::UserDTO(const std::string& id, const std::string& name, const std::string& email,
                 const std::string& password)
    : _id(id), _name(name), _email(email), _password(password) {}

UserDTO::~UserDTO() = default;

std::string UserDTO::getId() const {
    return _id;
}

std::string UserDTO::getName() const {
    return _name;
}

std::string UserDTO::getEmail() const {
    return _email;
}

std::string UserDTO::getPassword() const {
    return _password;
}

// TODO: Change to use hashing
bool UserDTO::authenticate(const std::string& inputPass) const {
    return _password == inputPass;
}
