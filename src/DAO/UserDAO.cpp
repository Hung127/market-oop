#include "../../include/DAO/UserDAO.h"

#include <expected>
#include <memory>
#include <string>

#include "../../include/DTO/UserDTO.h"

std::expected<std::shared_ptr<UserDTO>, std::string>
UserDAO::getUserByEmail(const std::string& email) {
    for (auto user : UserDAO::_users) {
        if (!user) {
            continue;
        }
        if (email == user->getEmail()) {
            return user;
        }
    }
    return std::unexpected("There is no user with that email");
}

std::expected<void, std::string> UserDAO::addUser(const std::shared_ptr<UserDTO>& user) {
    for (auto userDb : UserDAO::_users) {
        if (!userDb) {
            continue;
        }
        if (user->getId() == userDb->getId()) {
            return std::unexpected("Already have that id");
        }
        if (user->getEmail() == userDb->getEmail()) {
            return std::unexpected("Already have that email");
        }
    }

    UserDAO::_users.push_back(user);

    return {};
}
