#include "../../include/DTO/UserDTO.h"

UserDTO::UserDTO(const std::string& id, const std::string& name, const std::string& email,
                 const std::string& hashedPassword)
    : _id(id), _name(name), _email(email), _hashedPassword(hashedPassword) {
    UserDTO::ids.push_back(id);
}

std::string UserDTO::getId() const {
    return _id;
}

std::string UserDTO::getName() const {
    return _name;
}

const std::vector<std::string>& UserDTO::getIDS() {
    return UserDTO::ids;
}

std::string UserDTO::getEmail() const {
    return _email;
}

std::string UserDTO::getPassword() const {
    return _hashedPassword;
}

bool UserDTO::authenticate(const std::string& hashedInput) const {
    return _hashedPassword == hashedInput;
}
