#include "../../include/DTO/UserDTO.h"

UserDTO::UserDTO(const std::string& id, const std::string& name, const std::string& email,
                 const std::string& hashedPassword)
    : _id(id), _name(name), _email(email), _hashedPassword(hashedPassword) {
    UserDTO::ids.push_back(id);
}

const std::string& UserDTO::getId() const {
    return _id;
}

const std::string& UserDTO::getName() const {
    return _name;
}

const std::vector<std::string>& UserDTO::getIDS() {
    return UserDTO::ids;
}

const std::string& UserDTO::getEmail() const {
    return _email;
}

const std::string& UserDTO::getHashedPassword() const {
    return _hashedPassword;
}
