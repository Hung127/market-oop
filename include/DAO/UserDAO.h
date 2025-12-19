#ifndef USERDAO_H
#define USERDAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DTO/UserDTO.h"

class UserDAO {
   private:
    inline static std::vector<std::shared_ptr<UserDTO>> _users;

   public:
    static std::expected<std::shared_ptr<UserDTO>, std::string>
    getUserByEmail(const std::string& email);
    static std::expected<void, std::string> addUser(const std::shared_ptr<UserDTO>& user);
    static bool existById(const std::string& id);
    static bool existByEmail(const std::string& email);
};

#endif
