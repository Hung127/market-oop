#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include "../include/BUS/BuyerBUS.h"
#include "../include/BUS/CartBUS.h"
#include "../include/BUS/ProductBUS.h"
#include "../include/BUS/SellerBUS.h"
#include "../include/BUS/UserBUS.h"
#include "../include/DAO/BuyerDAO.h"
#include "../include/DAO/OrderDAO.h"
#include "../include/DAO/ProductDAO.h"
#include "../include/DAO/SellerDAO.h"
#include "../include/DAO/UserDAO.h"
#include "../include/DTO/BuyerDTO.h"
#include "../include/DTO/OrderDTO.h"
#include "../include/DTO/ProductDTO.h"
#include "../include/DTO/SellerDTO.h"
#include "../include/UserFactory.h"
#include "../include/Utils/Utils.h"
#include "../include/enums/OrderItemStatus.h"

// Lightweight test helpers used across test files.
// Keep them inline/static to avoid ODR violations when included in multiple translation units.

// Ensure that a seller (user) exists in DB and return a shared_ptr<SellerDTO>.
// If an existing user with the same id already exists and is a seller, return that instance.
// Otherwise insert the user via UserDAO and return the newly-created DTO.
// If id collides with a non-seller user, a unique id will be generated.
static inline std::shared_ptr<SellerDTO> makeSeller(const std::string& id,
                                                    const std::string& name) {
    // If a user with that id already exists, try to return it if it's a SellerDTO
    if (UserDAO::existById(id)) {
        auto existingPack = UserDAO::getUserById(id);
        if (existingPack.has_value()) {
            if (auto existingSeller = std::dynamic_pointer_cast<SellerDTO>(existingPack.value())) {
                return existingSeller;
            }
        }
        // If exist but not a seller (or couldn't be constructed), fall back to unique id below.
    }

    // If the id is already used as an email etc., choose a stable unique id
    std::string finalId = id;
    if (UserDAO::existById(finalId) || UserDAO::existByEmail(finalId + "@test.com")) {
        finalId = id + "-" + Utils::generateId();
    }

    auto seller = std::make_shared<SellerDTO>(finalId, name, finalId + "@test.com", "pw");

    // Insert seller into DB; ignore failure if another test inserted it concurrently.
    auto addRes = UserDAO::addUser(seller);
    (void)
        addRes;  // tests will still work if addUser reports duplicate; we always return seller DTO

    return seller;
}

// Create product DTO, ensure seller exists in DB first, insert product into ProductDAO.
// If provided id collides, produce a unique id by appending a generated suffix.
static inline std::shared_ptr<ProductDTO> makeProduct(const std::string& id,
                                                      const std::string& name, double price,
                                                      int stock,
                                                      std::shared_ptr<SellerDTO> seller) {
    if (!seller) {
        throw std::runtime_error("makeProduct requires a non-null seller");
    }

    // Ensure seller exists in DB (UserDAO)
    if (!UserDAO::existById(seller->getId())) {
        auto saveRes = UserDAO::addUser(seller);
        (void)saveRes;  // ignore error - if it fails we will attempt insert anyway
    }

    std::string finalId = id;
    if (ProductDAO::exists(finalId)) {
        // If a product with this id already exists in DB, create a unique id instead.
        finalId = id + "-" + Utils::generateId();
    }

    auto p = std::make_shared<ProductDTO>(finalId, name, price, stock, seller);

    // Keep seller inventory consistent (in-memory)
    seller->addProduct(p);

    // Try to insert product into DB; if insertion fails because of any reason attempt again with
    // unique suffix.
    if (!ProductDAO::insert(p)) {
        // try unique suffix once more
        finalId = id + "-" + Utils::generateId();
        p = std::make_shared<ProductDTO>(finalId, name, price, stock, seller);
        seller->addProduct(p);
        ProductDAO::insert(p);  // ignore return, tests will still use p
    }

    return p;
}

static inline std::shared_ptr<BuyerDTO> makeBuyer(const std::string& name, double balance) {
    std::string email = name + "+test@" + Utils::generateId() + ".com";
    auto pack = UserBUS::registerUser(UserRole::BUYER, name, email, "password123", balance);
    if (!pack.has_value())
        throw std::runtime_error("Failed to create Buyer.");
    return std::dynamic_pointer_cast<BuyerDTO>(pack.value());
}

#endif  // TESTS_COMMON_H
