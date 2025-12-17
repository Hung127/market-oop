#ifndef USERDAO_H
#define USERDAO_H

#include <expected>
#include <memory>
#include <string>

#include "../DTO/UserDTO.h"
#include "BuyerDAO.h"

class UserDAO {
   public:
    static std::expected<std::unique_ptr<UserDTO>, std::string> login(const std::string& email,
                                                                      const std::string& password) {
        if (email != password) {
            return std::unexpected(
                "Email hoac mat khau khong dung (Hoac chuc nang Login chua noi Database)");
        }
        return std::unexpected(
            "Email hoac mat khau khong dung (Hoac chuc nang Login chua noi Database)");
    }
};

#endif
