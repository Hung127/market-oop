#include "../../include/BUS/UserBUS.h"

#include <format>
#include <memory>
#include <string>

#include "../../include/DAO/UserDAO.h"
#include "../../include/DTO/UserDTO.h"
#include "../../include/UserFactory.h"
#include "../../include/Utils/Utils.h"

std::expected<std::shared_ptr<UserDTO>, std::string> UserBUS::login(const std::string& email,
                                                                    const std::string& password) {
    using Utils::PasswordUtils;  // for short statements

    auto userPack = UserDAO::getUserByEmail(email);
    if (!userPack.has_value()) {
        return std::unexpected("There is no user with that email");
    }

    std::shared_ptr<UserDTO> user = userPack.value();
    if (!user) {
        return std::unexpected("Invalid user with that email");
    }

    auto hashedPassword = user->getHashedPassword();
    // success
    if (PasswordUtils::verify(password, hashedPassword)) {
        return user;
    }

    return std::unexpected("Invalid email or password");
}

std::expected<std::shared_ptr<UserDTO>, std::string>
UserBUS::registerUser(UserRole role, const std::string& name, const std::string& email,
                      const std::string& password, double initialBalance) {
    using Utils::PasswordUtils;

    if (password.length() < PasswordUtils::MIN_PASSWORD_LENGTH) {
        std::string error = std::format("The password need to have at least {} characters length",
                                        PasswordUtils::MIN_PASSWORD_LENGTH);
        return std::unexpected(error);
    }

    std::string newId = "0000";  // Utils::generateId(); TODO: Complete this function

    std::string hashedPassword = Utils::hashPassword(password);
    return UserFactory::createUser(role, newId, name, email, hashedPassword, initialBalance);
}
