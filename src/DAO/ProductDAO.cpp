#include "../../include/DAO/ProductDAO.h"

#include <iostream>

#include <sqlite3.h>

#include "../../include/DAO/UserDAO.h"
#include "../../include/DTO/ProductDTO.h"
#include "../../include/DTO/SellerDTO.h"
#include "../../include/database/DatabaseManager.h"

std::shared_ptr<DatabaseManager> ProductDAO::_dbManager = nullptr;

void ProductDAO::setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager) {
    _dbManager = dbManager;
}

std::expected<std::shared_ptr<ProductDTO>, std::string>
ProductDAO::getProductById(const std::string& id) {
    if (!_dbManager) {
        return std::unexpected("Database not initialized");
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, price, stock, seller_id FROM products WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare statement");
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

        auto product = std::make_shared<ProductDTO>(productId, name, price, stock, seller);

        return product;
    }

    sqlite3_finalize(stmt);
    return std::unexpected("Product not found with ID: " + id);
}

std::vector<std::shared_ptr<ProductDTO>> ProductDAO::getAllProducts() {
    std::vector<std::shared_ptr<ProductDTO>> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "SELECT id, name, price, stock, seller_id FROM products ORDER BY created_at DESC";

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

std::vector<std::shared_ptr<ProductDTO>>
ProductDAO::getProductsBySeller(const std::string& sellerId) {
    std::vector<std::shared_ptr<ProductDTO>> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, price, stock, seller_id FROM products WHERE seller_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, sellerId.c_str(), -1, SQLITE_TRANSIENT);

    // Get seller once
    auto sellerResult = UserDAO::getUserById(sellerId);
    std::shared_ptr<SellerDTO> seller = nullptr;
    if (sellerResult.has_value()) {
        seller = std::dynamic_pointer_cast<SellerDTO>(sellerResult.value());
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);

        auto product = std::make_shared<ProductDTO>(id, name, price, stock, seller);
        result.push_back(product);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool ProductDAO::insert(const std::shared_ptr<ProductDTO>& product) {
    if (!_dbManager || !product) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "INSERT INTO products (id, name, price, stock, seller_id) VALUES (?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Failed to prepare insert statement: " << sqlite3_errmsg(db)
                  << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, product->getID().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, product->getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, product->getPrice());
    sqlite3_bind_int(stmt, 4, product->getStock());
    sqlite3_bind_text(stmt, 5, product->getSellerId().c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;

    if (!success) {
        std::cerr << "[ERROR] Failed to insert product: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

bool ProductDAO::insert(const ProductDTO& product) {
    return insert(std::make_shared<ProductDTO>(product));
}

bool ProductDAO::update(const ProductDTO& product) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE products 
        SET name = ?, price = ?, stock = ?, updated_at = CURRENT_TIMESTAMP 
        WHERE id = ? 
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, product.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 2, product.getPrice());
    sqlite3_bind_int(stmt, 3, product.getStock());
    sqlite3_bind_text(stmt, 4, product.getID().c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool ProductDAO::remove(const std::string& productId) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();

    // Check if product exists first
    if (!exists(productId)) {
        return false;
    }

    // Begin transaction
    if (!_dbManager->beginTransaction()) {
        return false;
    }

    sqlite3_stmt* stmt;

    // First, check if product is referenced in any non-cancelled order items
    const char* checkSql = R"(
        SELECT COUNT(*) FROM order_items 
        WHERE product_id = ?  AND status != 'cancelled'
    )";

    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr) != SQLITE_OK) {
        _dbManager->rollbackTransaction();
        return false;
    }

    sqlite3_bind_text(stmt, 1, productId.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (count > 0) {
        // Product is referenced in active orders, cannot delete
        _dbManager->rollbackTransaction();
        std::cerr << "[WARN] Cannot delete product " << productId
                  << " - referenced in active orders" << std::endl;
        return false;
    }

    // Safe to delete
    const char* sql = "DELETE FROM products WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        _dbManager->rollbackTransaction();
        return false;
    }

    sqlite3_bind_text(stmt, 1, productId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    int rowsAffected = sqlite3_changes(db);
    sqlite3_finalize(stmt);

    if (success && rowsAffected > 0) {
        _dbManager->commitTransaction();
        return true;
    }

    _dbManager->rollbackTransaction();
    return false;
}

std::vector<std::shared_ptr<ProductDTO>> ProductDAO::searchByName(const std::string& keyword) {
    std::vector<std::shared_ptr<ProductDTO>> result;

    if (!_dbManager || keyword.empty()) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, price, stock, seller_id FROM products WHERE name LIKE ? ";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    std::string pattern = "%" + keyword + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        std::string sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

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

std::vector<std::shared_ptr<ProductDTO>> ProductDAO::getProductsByPriceRange(double minPrice,
                                                                             double maxPrice) {
    std::vector<std::shared_ptr<ProductDTO>> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "SELECT id, name, price, stock, seller_id FROM products WHERE price BETWEEN ? AND ? ";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_double(stmt, 1, minPrice);
    sqlite3_bind_double(stmt, 2, maxPrice);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double price = sqlite3_column_double(stmt, 2);
        int stock = sqlite3_column_int(stmt, 3);
        std::string sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

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

bool ProductDAO::updateStock(const std::string& productId, int newStock) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "UPDATE products SET stock = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, newStock);
    sqlite3_bind_text(stmt, 2, productId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool ProductDAO::decrementStock(const std::string& productId, int quantity) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE products 
        SET stock = stock - ?, updated_at = CURRENT_TIMESTAMP 
        WHERE id = ?  AND stock >= ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_text(stmt, 2, productId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, quantity);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success && sqlite3_changes(db) > 0;
}

bool ProductDAO::incrementStock(const std::string& productId, int quantity) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE products 
        SET stock = stock + ?, updated_at = CURRENT_TIMESTAMP 
        WHERE id = ? 
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, quantity);
    sqlite3_bind_text(stmt, 2, productId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

int ProductDAO::getProductCount() {
    if (!_dbManager) {
        return 0;
    }

    return _dbManager->executeCount("SELECT COUNT(*) FROM products");
}

int ProductDAO::getProductCountBySeller(const std::string& sellerId) {
    if (!_dbManager) {
        return 0;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT COUNT(*) FROM products WHERE seller_id = ? ";

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

bool ProductDAO::exists(const std::string& productId) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT COUNT(*) FROM products WHERE id = ? ";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, productId.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}
