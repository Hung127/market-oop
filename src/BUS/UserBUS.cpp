#include "../../include/BUS/UserBUS.h"

#include <string>

#include "../../include/DAO/UserDAO.h"
#include "../../include/UserFactory.h"

std::expected<std::unique_ptr<UserDTO>, std::string> UserBUS::login(const std::string& email,
                                                                    const std::string& password) {
    // do nothing
    return UserDAO::login(email, password);
}

std::expected<std::unique_ptr<UserDTO>, std::string>
UserBUS::registerUser(UserRole role, const std::string& name, const std::string& email,
                      const std::string& password, double initialBalance) {
    if (password.length() < 6) {
        return std::unexpected("Mat khau phai dai hon 6 ky tu!");
    }

    std::string newId = "0000";  // Utils::generateId(); TODO: Complete this function
    return UserFactory::createUser(role, newId, name, email, password, initialBalance);
}
