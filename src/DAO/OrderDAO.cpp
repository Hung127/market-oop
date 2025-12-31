#include "../../include/DAO/OrderDAO.h"

#include <iostream>

#include <sqlite3.h>

#include "../../include/DAO/ProductDAO.h"
#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"
#include "../../include/database/DatabaseManager.h"
#include "../../include/enums/OrderItemStatus.h"

std::shared_ptr<DatabaseManager> OrderDAO::_dbManager = nullptr;

void OrderDAO::setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager) {
    _dbManager = dbManager;
}

std::vector<std::shared_ptr<OrderItemDTO>> OrderDAO::loadOrderItems(const std::string& orderId) {
    std::vector<std::shared_ptr<OrderItemDTO>> items;

    if (!_dbManager) {
        return items;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        SELECT oi.id, oi.product_id, oi. product_name, oi.seller_id, u.name as seller_name, 
               oi.price, oi.quantity, oi.subtotal, oi.status
        FROM order_items oi
        LEFT JOIN users u ON oi.seller_id = u.id
        WHERE oi.order_id = ?  
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return items;
    }

    sqlite3_bind_text(stmt, 1, orderId.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int itemId = sqlite3_column_int(stmt, 0);
        std::string productId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string productName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string sellerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string sellerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        double price = sqlite3_column_double(stmt, 5);
        int quantity = sqlite3_column_int(stmt, 6);
        std::string statusStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));

        OrderItemStatus status = stringToOrderItemStatus(statusStr);

        auto item = std::make_shared<OrderItemDTO>(productId, productName, sellerId, sellerName,
                                                   price, quantity);
        item->setId(itemId);
        item->setStatus(status);

        items.push_back(item);
    }

    sqlite3_finalize(stmt);
    return items;
}

std::expected<void, std::string> OrderDAO::addOrder(const OrderDTO& order) {
    if (!_dbManager) {
        return std::unexpected("Database not initialized");
    }

    sqlite3* db = _dbManager->getConnection();

    // Start transaction
    if (!_dbManager->beginTransaction()) {
        return std::unexpected("Failed to begin transaction");
    }

    // 1. Insert order
    sqlite3_stmt* stmt;
    const char* orderSql = R"(
        INSERT INTO orders (id, buyer_id, total_amount, status, created_at)
        VALUES (?, ?, ?, 'pending', ?)
    )";

    if (sqlite3_prepare_v2(db, orderSql, -1, &stmt, nullptr) != SQLITE_OK) {
        _dbManager->rollbackTransaction();
        return std::unexpected("Failed to prepare order insert");
    }

    sqlite3_bind_text(stmt, 1, order.orderId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, order.buyerId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 3, order.totalPrice());
    sqlite3_bind_text(stmt, 4, order.date().c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string error = sqlite3_errmsg(db);
        std::cerr << "[ERROR] Failed to insert order: " << error << std::endl;
        sqlite3_finalize(stmt);
        _dbManager->rollbackTransaction();
        return std::unexpected("Failed to insert order:  " + error);
    }
    sqlite3_finalize(stmt);

    // 2. Insert order items
    const char* itemSql = R"(
        INSERT INTO order_items (order_id, product_id, product_name, seller_id, quantity, price, subtotal, status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?)
    )";

    for (const auto& item : order.items()) {
        if (!item)
            continue;

        if (sqlite3_prepare_v2(db, itemSql, -1, &stmt, nullptr) != SQLITE_OK) {
            _dbManager->rollbackTransaction();
            return std::unexpected("Failed to prepare order item insert");
        }

        double subtotal = item->getSubtotal();
        std::string statusStr = orderItemStatusToString(item->getStatus());

        sqlite3_bind_text(stmt, 1, order.orderId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, item->getProductId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, item->getProductName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, item->getSellerId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, item->getQuantity());
        sqlite3_bind_double(stmt, 6, item->getPrice());
        sqlite3_bind_double(stmt, 7, subtotal);
        sqlite3_bind_text(stmt, 8, statusStr.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            _dbManager->rollbackTransaction();
            return std::unexpected("Failed to insert order item");
        }
        sqlite3_finalize(stmt);
    }

    // Commit transaction
    if (!_dbManager->commitTransaction()) {
        return std::unexpected("Failed to commit transaction");
    }

    return {};
}

std::expected<void, std::string> OrderDAO::updateOrder(const OrderDTO& order) {
    if (!_dbManager) {
        return std::unexpected("Database not initialized");
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE orders 
        SET total_amount = ?, updated_at = CURRENT_TIMESTAMP
        WHERE id = ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare update");
    }

    sqlite3_bind_double(stmt, 1, order.totalPrice());
    sqlite3_bind_text(stmt, 2, order.orderId().c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    if (!success) {
        return std::unexpected("Failed to update order");
    }

    return {};
}

std::shared_ptr<OrderDTO> OrderDAO::getOrderById(const std::string& id) {
    if (!_dbManager) {
        return nullptr;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "SELECT id, buyer_id, total_amount, status, created_at FROM orders WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string orderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string buyerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double totalAmount = sqlite3_column_double(stmt, 2);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        sqlite3_finalize(stmt);

        // Load order items
        auto items = loadOrderItems(orderId);

        auto order = std::make_shared<OrderDTO>(orderId, buyerId, items, totalAmount, date);

        return order;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

std::vector<std::shared_ptr<OrderDTO>> OrderDAO::getOrdersByBuyerId(const std::string& buyerId) {
    std::vector<std::shared_ptr<OrderDTO>> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "SELECT id, buyer_id, total_amount, created_at FROM orders WHERE buyer_id = ?  ORDER BY "
        "created_at DESC";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, buyerId.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string orderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string buyerIdDb = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double totalAmount = sqlite3_column_double(stmt, 2);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        // Load order items
        auto items = loadOrderItems(orderId);

        auto order = std::make_shared<OrderDTO>(orderId, buyerIdDb, items, totalAmount, date);
        result.push_back(order);
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::shared_ptr<OrderDTO>> OrderDAO::getOrdersBySellerId(const std::string& sellerId) {
    std::vector<std::shared_ptr<OrderDTO>> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        SELECT DISTINCT o.id, o.buyer_id, o.total_amount, o.created_at
        FROM orders o
        JOIN order_items oi ON o.id = oi.order_id
        WHERE oi.seller_id = ?  
        ORDER BY o.created_at DESC
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, sellerId.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string orderId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string buyerId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        double totalAmount = sqlite3_column_double(stmt, 2);
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        auto items = loadOrderItems(orderId);

        auto order = std::make_shared<OrderDTO>(orderId, buyerId, items, totalAmount, date);
        result.push_back(order);
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<OrderItemDTO> OrderDAO::getSellerOrderItemByStatus(const std::string& sellerId,
                                                               OrderItemStatus status) {
    std::vector<OrderItemDTO> result;

    if (!_dbManager) {
        return result;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        SELECT oi.id, oi.product_id, oi.product_name, oi.seller_id, u.name as seller_name, 
               oi.price, oi.quantity, oi.status
        FROM order_items oi
        LEFT JOIN users u ON oi.seller_id = u. id
        WHERE oi.seller_id = ? AND oi. status = ? 
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    std::string statusStr = orderItemStatusToString(status);

    sqlite3_bind_text(stmt, 1, sellerId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, statusStr.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int itemId = sqlite3_column_int(stmt, 0);
        std::string productId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string productName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string sellerIdDb = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string sellerName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        double price = sqlite3_column_double(stmt, 5);
        int quantity = sqlite3_column_int(stmt, 6);

        OrderItemDTO item(productId, productName, sellerIdDb, sellerName, price, quantity);
        item.setId(itemId);
        item.setStatus(status);

        result.push_back(item);
    }

    sqlite3_finalize(stmt);
    return result;
}

bool OrderDAO::updateOrderItemStatus(int itemId, OrderItemStatus newStatus) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE order_items 
        SET status = ?, updated_at = CURRENT_TIMESTAMP
        WHERE id = ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::string statusStr = orderItemStatusToString(newStatus);

    sqlite3_bind_text(stmt, 1, statusStr.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, itemId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool OrderDAO::updateOrderItem(const OrderItemDTO& item) {
    if (!_dbManager || item.getId() < 0) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE order_items 
        SET status = ?, quantity = ?, price = ?, subtotal = ?, updated_at = CURRENT_TIMESTAMP
        WHERE id = ?
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    std::string statusStr = orderItemStatusToString(item.getStatus());

    sqlite3_bind_text(stmt, 1, statusStr.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, item.getQuantity());
    sqlite3_bind_double(stmt, 3, item.getPrice());
    sqlite3_bind_double(stmt, 4, item.getSubtotal());
    sqlite3_bind_int(stmt, 5, item.getId());

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

bool OrderDAO::updateOrderStatus(const std::string& orderId, const std::string& status) {
    if (!_dbManager) {
        return false;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        UPDATE orders 
        SET status = ?, updated_at = CURRENT_TIMESTAMP
        WHERE id = ?  
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, orderId.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    return success;
}

int OrderDAO::getOrderCount() {
    if (!_dbManager) {
        return 0;
    }
    return _dbManager->executeCount("SELECT COUNT(*) FROM orders");
}

int OrderDAO::getOrderCountByBuyer(const std::string& buyerId) {
    if (!_dbManager) {
        return 0;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT COUNT(*) FROM orders WHERE buyer_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_text(stmt, 1, buyerId.c_str(), -1, SQLITE_TRANSIENT);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

double OrderDAO::getTotalRevenueBySeller(const std::string& sellerId) {
    if (!_dbManager) {
        return 0.0;
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = R"(
        SELECT SUM(subtotal)
        FROM order_items
        WHERE seller_id = ? AND status != 'cancelled'
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return 0.0;
    }

    sqlite3_bind_text(stmt, 1, sellerId.c_str(), -1, SQLITE_TRANSIENT);

    double revenue = 0.0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        revenue = sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return revenue;
}
