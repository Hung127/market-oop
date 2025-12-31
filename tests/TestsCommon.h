#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <iostream>
#include <memory>
#include <stdexcept>
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
static inline std::unordered_map<std::string, std::shared_ptr<ProductDTO>> g_testProducts;

static inline std::shared_ptr<SellerDTO> makeSeller(const std::string& id,
                                                    const std::string& name) {
    // Create the seller DTO (in-memory)
    auto seller = std::make_shared<SellerDTO>(id, name, id + "@test.com", "pw");

    // Try to persist into DB. If it fails because it already exists, fetch existing one.
    auto addRes = UserDAO::addUser(seller);
    if (!addRes.has_value()) {
        // Try load existing seller by id (if present)
        auto existing = UserDAO::getUserById(id);
        if (existing.has_value()) {
            auto existingSeller = std::dynamic_pointer_cast<SellerDTO>(existing.value());
            if (existingSeller) {
                return existingSeller;
            }
        }
        // If we reach here, either add failed for another reason or we couldn't fetch;
        // continue with in-memory seller (some tests only need the DTO in memory).
    }

    return seller;
}

inline std::shared_ptr<ProductDTO> makeProduct(const std::string& id, const std::string& name,
                                               double price, int stock,
                                               const std::shared_ptr<SellerDTO>& seller) {
    // Check if product already exists in DB
    if (ProductDAO::exists(id)) {
        // Remove it first
        ProductDAO::remove(id);
    }

    auto product = std::make_shared<ProductDTO>(id, name, price, stock, seller);

    if (!ProductDAO::insert(product)) {
        std::cerr << "[ERROR] Failed to insert product " << id << std::endl;
    }

    g_testProducts[id] = product;
    return product;
}

static inline std::shared_ptr<BuyerDTO> makeBuyer(const std::string& name, double balance) {
    std::string email = name + "+test@" + Utils::generateId() + ".com";
    auto pack = UserBUS::registerUser(UserRole::BUYER, name, email, "password123", balance);
    if (!pack.has_value())
        throw std::runtime_error("Failed to create Buyer: " + pack.error());

    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(pack.value());

    // Verify buyer exists in DB
    if (!UserDAO::existById(buyer->getId())) {
        throw std::runtime_error("Buyer not persisted to DB: " + buyer->getId());
    }

    BuyerDAO::save(*buyer);

    return buyer;
}

#endif  // TESTS_COMMON_H
