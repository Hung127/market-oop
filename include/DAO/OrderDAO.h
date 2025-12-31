#ifndef ORDER_DAO_H
#define ORDER_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../enums/OrderItemStatus.h"

class OrderDTO;
class OrderItemDTO;
class DatabaseManager;

class OrderDAO {
   private:
    static std::shared_ptr<DatabaseManager> _dbManager;

    // Helper to load order items for an order
    static std::vector<std::shared_ptr<OrderItemDTO>> loadOrderItems(const std::string& orderId);

   public:
    static void setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager);

    // Order operations
    static std::expected<void, std::string> addOrder(const OrderDTO& order);
    static std::expected<void, std::string> updateOrder(const OrderDTO& order);

    static std::shared_ptr<OrderDTO> getOrderById(const std::string& id);

    static std::vector<std::shared_ptr<OrderDTO>> getOrdersByBuyerId(const std::string& buyerId);

    static std::vector<std::shared_ptr<OrderDTO>> getOrdersBySellerId(const std::string& sellerId);

    // Order item operations
    static std::vector<OrderItemDTO> getSellerOrderItemByStatus(const std::string& sellerId,
                                                                OrderItemStatus status);

    static bool updateOrderItemStatus(int itemId, OrderItemStatus newStatus);

    static bool updateOrderItem(const OrderItemDTO& item);

    static bool updateOrderStatus(const std::string& orderId, const std::string& status);

    // Statistics
    static int getOrderCount();
    static int getOrderCountByBuyer(const std::string& buyerId);
    static double getTotalRevenueBySeller(const std::string& sellerId);
};

#endif
