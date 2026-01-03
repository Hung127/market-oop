#ifndef USERBUS_H
#define USERBUS_H

#include <expected>
#include <memory>
#include <string>

#include "../UserFactory.h"  // defines UserRole

class UserDTO;

class UserBUS {
   public:
    // Trả về shared_ptr<UserDTO> hoặc error string
    static std::expected<std::shared_ptr<UserDTO>, std::string> login(const std::string& email,
                                                                      const std::string& password);

    static std::expected<std::shared_ptr<UserDTO>, std::string>

    registerUser(UserRole role, const std::string& name, const std::string& email,
                 const std::string& password, double initialBalance = 0);

    static void logout(std::shared_ptr<UserDTO>& user);
};

#endif  // USERBUS_H
