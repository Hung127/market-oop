#include "../../include/DTO/UserDTO.h"

#include <memory>

#include "../../include/DAO/UserDAO.h"

UserDTO::UserDTO(const std::string& id, const std::string& name, const std::string& email,
                 const std::string& hashedPassword)
    : _id(id), _name(name), _email(email), _hashedPassword(hashedPassword) {
    // Do nothing
}

const std::string& UserDTO::getId() const {
    return _id;
}

const std::string& UserDTO::getName() const {
    return _name;
}

const std::string& UserDTO::getEmail() const {
    return _email;
}

const std::string& UserDTO::getHashedPassword() const {
    return _hashedPassword;
}
