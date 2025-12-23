#include "../../include/DAO/BuyerDAO.h"

#include <memory>
#include <string>

#include "../../include/DAO/UserDAO.h"
#include "../../include/DTO/BuyerDTO.h"

bool BuyerDAO::save(const BuyerDTO& buyer) {
    auto userPack = UserDAO::getUserById(buyer.getId());

    if (userPack.has_value()) {
        auto user = userPack.value();
        if (auto buyerPtr = std::dynamic_pointer_cast<BuyerDTO>(user)) {
            // Update the existing buyer (copy data into shared instance)
            *buyerPtr = buyer;
            return true;
        }
    }

    // If not found, add as new user to UserDAO
    auto newBuyer = std::make_shared<BuyerDTO>(buyer);
    auto addResult = UserDAO::addUser(newBuyer);
    return addResult.has_value();
}

std::shared_ptr<BuyerDTO> BuyerDAO::getBuyerById(const std::string& id) {
    auto userPack = UserDAO::getUserById(id);

    if (userPack.has_value()) {
        return std::dynamic_pointer_cast<BuyerDTO>(userPack.value());
    }

    return nullptr;
}
