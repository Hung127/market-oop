#ifndef USER_BUS_H
#define USER_BUS_H

#include <string>
#include <expected>
#include <memory>
#include "User_DAO.h"
#include "UserFactory.h"

class UserBus {
public:
    static std::expected<std::unique_ptr<UserDto>, std::string> 
    login(const std::string& email, const std::string& password) {
        if (email.empty() || password.empty()) {
            return std::unexpected("Email va mat khau khong duoc de trong!");
        }

        return UserDao::login(email, password);
    }

    static std::expected<std::unique_ptr<UserDto>, std::string>
    registerUser(UserRole role, const std::string& name, const std::string& email, 
                 const std::string& password, double initialBalance = 0) {
        
        if (password.length() < 6) {
            return std::unexpected("Mat khau phai dai hon 6 ky tu!");
        }

        std::string newId = Utils::generateId(); 
        return UserFactory::createUser(role, newId, name, email, password, initialBalance);
    }
};

#endif // USER_BUS_H
