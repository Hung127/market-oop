#include "../../include/DAO/SellerDAO.h"

#include <iostream>

#include <sqlite3.h>

#include "../../include/DAO/OrderDAO.h"
#include "../../include/DAO/ProductDAO.h"
#include "../../include/DAO/UserDAO.h"
#include "../../include/DTO/SellerDTO.h"
#include "../../include/database/DatabaseManager.h"

std::shared_ptr<DatabaseManager> SellerDAO::_dbManager = nullptr;

void SellerDAO::setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager) {
    _dbManager = dbManager;
}

bool SellerDAO::save(const SellerDTO& seller) {
    // Use UserDAO to save the seller (since seller is a user)
    auto userPack = UserDAO::getUserById(seller.getId());

    if (userPack.has_value()) {
        // User exists, update it
        return UserDAO::updateUser(std::make_shared<SellerDTO>(seller)).has_value();
    }

    // If not found, add as new user
    auto newSeller = std::make_shared<SellerDTO>(seller);
    auto addResult = UserDAO::addUser(newSeller);
    return addResult.has_value();
}

std::shared_ptr<SellerDTO> SellerDAO::getSellerById(const std::string& id) {
    auto userPack = UserDAO::getUserById(id);

    if (userPack.has_value()) {
        auto seller = std::dynamic_pointer_cast<SellerDTO>(userPack.value());

        // Load seller's products
        if (seller) {
            auto products = ProductDAO::getProductsBySeller(id);
            for (const auto& product : products) {
                seller->addProduct(product);
            }
        }

        return seller;
    }

    return nullptr;
}

std::vector<std::shared_ptr<SellerDTO>> SellerDAO::getAllSellers() {
    std::vector<std::shared_ptr<SellerDTO>> result;

    auto users = UserDAO::getUsersByType("seller");

    for (const auto& user : users) {
        if (auto seller = std::dynamic_pointer_cast<SellerDTO>(user)) {
            // Load seller's products
            auto products = ProductDAO::getProductsBySeller(seller->getId());
            for (const auto& product : products) {
                seller->addProduct(product);
            }

            result.push_back(seller);
        }
    }

    return result;
}

int SellerDAO::getProductCountForSeller(const std::string& sellerId) {
    return ProductDAO::getProductCountBySeller(sellerId);
}

double SellerDAO::getTotalRevenue(const std::string& sellerId) {
    return OrderDAO::getTotalRevenueBySeller(sellerId);
}

int SellerDAO::getTotalSalesCount(const std::string& sellerId) {
    if (!_dbManager) {
        return 0;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        SELECT COUNT(*)
        FROM order_items
        WHERE seller_id = ?  AND status != 'cancelled'
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_text(stmt, 1, sellerId.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}
