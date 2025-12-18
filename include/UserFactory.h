#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include <expected>
#include <memory>
#include <string>

enum class UserRole { BUYER, SELLER };

class UserDTO;

class UserFactory {
   public:
    static std::expected<std::shared_ptr<UserDTO>, std::string>
    createUser(UserRole role, const std::string& id, const std::string& name,
               const std::string& email, const std::string& hashedPassword, double balance = 0.0);
};

#endif  // USER_FACTORY_H
