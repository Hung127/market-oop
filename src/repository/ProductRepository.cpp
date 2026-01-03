#include "../../include/repository/ProductRepository.h"

#include <iostream>
#include <sqlite3.h>
#include <expected>

#include "../../include/DAO/UserDAO.h"  // For getting seller info
#include "../../include/DTO/ProductDTO.h"
#include "../../include/DTO/SellerDTO.h"

// Repository Pattern: Implementation of ProductRepository using dependency-injected DatabaseManager
// This replaces static DAO methods with instance-based access for better testability and decoupling.

std::expected<void, std::string> ProductRepository::addProduct(const std::shared_ptr<ProductDTO>& p) {
    if (!p) {
        return std::unexpected("Product is null");
    }

    sqlite3* db = _dbManager.getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "INSERT INTO products (id, name, price, stock, seller_id) VALUES (?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare insert statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, p->getID().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p->getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, p->getPrice());
    sqlite3_bind_int(stmt, 4, p->getStock());
    sqlite3_bind_text(stmt, 5, p->getOwner() ? p->getOwner()->getId().c_str() : "", -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return std::unexpected("Failed to insert product: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
    return {};
}

std::expected<void, std::string> ProductRepository::removeProduct(const std::string& id) {
    sqlite3* db = _dbManager.getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "DELETE FROM products WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare delete statement: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        return std::unexpected("Failed to delete product: " + std::string(sqlite3_errmsg(db)));
    }

    sqlite3_finalize(stmt);
    return {};
}

std::shared_ptr<ProductDTO> ProductRepository::findProductById(const std::string& id) const {
    sqlite3* db = _dbManager.getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, price, stock, seller_id FROM products WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string productId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        std::string sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        sqlite3_finalize(stmt);

        // Get seller to create product with owner
        auto sellerResult = UserDAO::getUserById(sellerId);
        std::shared_ptr<SellerDTO> seller = nullptr;
        if (sellerResult.has_value()) {
            seller = std::dynamic_pointer_cast<SellerDTO>(sellerResult.value());
        }

        return std::make_shared<ProductDTO>(productId, name, price, stock, seller);
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<std::shared_ptr<ProductDTO>> ProductRepository::searchByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<ProductDTO>> result;

    sqlite3* db = _dbManager.getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, price, stock, seller_id FROM products WHERE name LIKE ? ORDER BY created_at DESC";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    std::string likeKeyword = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, likeKeyword.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        std::string sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        // Get seller
        auto sellerResult = UserDAO::getUserById(sellerId);
        std::shared_ptr<SellerDTO> seller = nullptr;
        if (sellerResult.has_value()) {
            seller = std::dynamic_pointer_cast<SellerDTO>(sellerResult.value());
        }

        auto product = std::make_shared<ProductDTO>(id, name, price, stock, seller);
        result.push_back(product);
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::shared_ptr<ProductDTO>> ProductRepository::getAllProducts() const {
    std::vector<std::shared_ptr<ProductDTO>> result;

    sqlite3* db = _dbManager.getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, price, stock, seller_id FROM products ORDER BY created_at DESC";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        std::string sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        // Get seller
        auto sellerResult = UserDAO::getUserById(sellerId);
        std::shared_ptr<SellerDTO> seller = nullptr;
        if (sellerResult.has_value()) {
            seller = std::dynamic_pointer_cast<SellerDTO>(sellerResult.value());
        }

        auto product = std::make_shared<ProductDTO>(id, name, price, stock, seller);
        result.push_back(product);
    }

    sqlite3_finalize(stmt);
    return result;
}

int ProductRepository::getProductCount() const {
    sqlite3* db = _dbManager.getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT COUNT(*) FROM products";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}