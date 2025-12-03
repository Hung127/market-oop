#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include <memory>
#include <string>
#include <expected>

#include "User_DTO.h"
#include "Buyer_DTO.h" 
#include "Seller_DTO.h"

enum class UserRole { 
    BUYER, 
    SELLER 
};

class UserFactory {
public:
    static std::expected<std::unique_ptr<UserDto>, std::string>
    createUser(UserRole role, const std::string& id, const std::string& name,
               const std::string& email, const std::string& password,
               double balance = 0.0) {

        if (role == UserRole::BUYER) {
            return std::make_unique<BuyerDto>(id, name, email, password, balance);
        }

        if (role == UserRole::SELLER) {
           
            return std::make_unique<SellerDto>(id, name, email, password);
        }

        return std::unexpected("Role khong hop le!");
    }
};

#endif // USER_FACTORY_H
