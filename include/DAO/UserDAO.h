#ifndef USERDAO_H
#define USERDAO_H

#include <expected>
#include <memory>
#include <string>

#include "../DTO/UserDTO.h"
#include "BuyerDAO.h"

class UserDAO {
   public:
    static std::expected<std::shared_ptr<UserDTO>, std::string>
    getUserByEmail(const std::string& email);
};

#endif
