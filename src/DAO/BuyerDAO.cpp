#include "../../include/DAO/BuyerDAO.h"

#include <iostream>

#include <sqlite3.h>

#include "../../include/DAO/UserDAO.h"
#include "../../include/DTO/BuyerDTO.h"
#include "../../include/database/DatabaseManager.h"

std::shared_ptr<DatabaseManager> BuyerDAO::_dbManager = nullptr;

void BuyerDAO::setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager) {
    _dbManager = dbManager;
}

bool BuyerDAO::save(const BuyerDTO& buyer) {
    // Use UserDAO to save the buyer (since buyer is a user)
    auto userPack = UserDAO::getUserById(buyer.getId());

    if (userPack.has_value()) {
        // User exists, update it
        return UserDAO::updateUser(std::make_shared<BuyerDTO>(buyer)).has_value();
    }

    // If not found, add as new user
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

std::vector<std::shared_ptr<BuyerDTO>> BuyerDAO::getAllBuyers() {
    std::vector<std::shared_ptr<BuyerDTO>> result;

    auto users = UserDAO::getUsersByType("buyer");

    for (const auto& user : users) {
        if (auto buyer = std::dynamic_pointer_cast<BuyerDTO>(user)) {
            result.push_back(buyer);
        }
    }

    return result;
}

bool BuyerDAO::addToCart(const std::string& buyerId, const std::string& productId, int quantity) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    // Use INSERT OR REPLACE to handle duplicates
    const char* sql = R"(
        INSERT INTO cart_items (buyer_id, product_id, quantity)
        VALUES (?, ?, ?)
        ON CONFLICT(buyer_id, product_id)
        DO UPDATE SET quantity = quantity + excluded.quantity
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to prepare cart insert: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, buyerId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, productId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, quantity);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool BuyerDAO::removeFromCart(const std::string& buyerId, const std::string& productId) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "DELETE FROM cart_items WHERE buyer_id = ? AND product_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, buyerId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, productId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool BuyerDAO::updateCartQuantity(const std::string& buyerId, const std::string& productId,
                                  int quantity) {
    if (!_dbManager) {
        return false;
    }

    if (quantity <= 0) {
        return removeFromCart(buyerId, productId);
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE cart_items 
        SET quantity = ? 
        WHERE buyer_id = ?  AND product_id = ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_text(stmt, 2, buyerId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, productId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool BuyerDAO::clearCart(const std::string& buyerId) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "DELETE FROM cart_items WHERE buyer_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, buyerId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

std::vector<std::pair<std::string, int>> BuyerDAO::getCartItems(const std::string& buyerId) {
    std::vector<std::pair<std::string, int>> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT product_id, quantity FROM cart_items WHERE buyer_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, buyerId.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string productId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int quantity = sqlite3_column_int(stmt, 1);

        result.push_back({productId, quantity});
    }

    sqlite3_finalize(stmt);
    return result;
}
