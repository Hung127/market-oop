#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

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

static inline std::shared_ptr<SellerDTO> makeSeller(const std::string& id,
                                                    const std::string& name) {
    return std::make_shared<SellerDTO>(id, name, id + "@test.com", "pw");
}

static inline std::shared_ptr<ProductDTO> makeProduct(const std::string& id,
                                                      const std::string& name, double price,
                                                      int stock,
                                                      std::shared_ptr<SellerDTO> seller) {
    auto p = std::make_shared<ProductDTO>(id, name, price, stock, seller);
    // Ensure seller inventory and product DAO both reference the same instance
    seller->addProduct(p);
    ProductDAO::insert(p);
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
#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

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

static inline std::shared_ptr<SellerDTO> makeSeller(const std::string& id,
                                                    const std::string& name) {
    return std::make_shared<SellerDTO>(id, name, id + "@test.com", "pw");
}

static inline std::shared_ptr<ProductDTO> makeProduct(const std::string& id,
                                                      const std::string& name, double price,
                                                      int stock,
                                                      std::shared_ptr<SellerDTO> seller) {
    auto p = std::make_shared<ProductDTO>(id, name, price, stock, seller);
    // Ensure seller inventory and product DAO both reference the same instance
    seller->addProduct(p);
    ProductDAO::insert(p);
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
