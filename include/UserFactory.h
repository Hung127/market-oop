#ifndef USER_FACTORY_H
#define USER_FACTORY_H

#include <memory>
#include <string>

#include "DTO/BuyerDTO.h"
#include "DTO/SellerDTO.h"
#include "DTO/UserDTO.h"

enum class UserRole { BUYER, SELLER };

class UserFactory {
   public:
    static std::expected<std::unique_ptr<UserDTO>, std::string>
    createUser(UserRole role, const std::string& id, const std::string& name,
               const std::string& email, const std::string& password, double balance = 0.0) {
        if (role == UserRole::BUYER) {
            std::unique_ptr<BuyerDTO> result(new BuyerDTO(id, name, email, password, balance));
            return result;
        }

        if (role == UserRole::SELLER) {
            return std::make_unique<SellerDTO>(id, name, email, password);
        }

        return std::unexpected("Role khong hop le!");
    }
};

#endif  // USER_FACTORY_H
