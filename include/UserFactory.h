#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include <expected>
#include <memory>

#include "Buyer.h"
#include "Seller.h"
#include "User.h"

enum class UserRole { BUYER, SELLER };

class UserFactory {
   public:
    static std::expected<std::unique_ptr<User>, std::string>
    createUser(UserRole role, const std::string& id, const std::string& name,
               const std::string& email, const std::string& password,
               double balance = 0.0);  // Optional params cho Buyer
};

#endif  // USER_FACTORY_H
