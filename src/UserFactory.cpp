#include "../include/UserFactory.h"

#include <memory>

#include "../include/DTO/BuyerDTO.h"
#include "../include/DTO/OrderDTO.h"
#include "../include/DTO/OrderItemDTO.h"
#include "../include/DTO/SellerDTO.h"

std::expected<std::shared_ptr<UserDTO>, std::string>
UserFactory::createUser(UserRole role, const std::string& id, const std::string& name,
                        const std::string& email, const std::string& password, double balance) {
    switch (role) {
        case UserRole::SELLER:
            return std::make_shared<SellerDTO>(SellerDTO(id, name, email, password));

        case UserRole::BUYER:
            return std::make_shared<BuyerDTO>(BuyerDTO(id, name, email, password, balance));

        default:
            return std::unexpected("Unknown user role");
    }
}
