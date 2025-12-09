#ifndef USERDAO_H
#define USERDAO_H

#include <string>
#include <memory>
#include <expected>
#include "User_DTO.h"
#include "Buyer_DAO.h"


class UserDao {
public:
    static std::expected<std::unique_ptr<UserDto>, std::string> 
    login(const std::string& email, const std::string& password) {
        return std::unexpected("Email hoac mat khau khong dung (Hoac chuc nang Login chua noi Database)");
    }
};

#endif
