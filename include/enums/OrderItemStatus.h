#ifndef ORDERITEMSTATUS_H
#define ORDERITEMSTATUS_H

#include <string>

enum class OrderItemStatus {
    PENDING,    // Order item created, awaiting seller action
    CONFIRMED,  // Seller confirmed, preparing to ship
    SHIPPED,    // Item dispatched
    DELIVERED,  // Item delivered to buyer
    CANCELLED   // Item cancelled
};

inline std::string orderItemStatusToString(OrderItemStatus status) {
    switch (status) {
        case OrderItemStatus::PENDING:
            return "pending";  // Changed from "Pending"
        case OrderItemStatus::CONFIRMED:
            return "confirmed";  // Changed from "Confirmed"
        case OrderItemStatus::SHIPPED:
            return "shipped";  // Changed from "Shipped"
        case OrderItemStatus::DELIVERED:
            return "delivered";  // Changed from "Delivered"
        case OrderItemStatus::CANCELLED:
            return "cancelled";  // Changed from "Cancelled"
        default:
            return "pending";
    }
}

inline OrderItemStatus stringToOrderItemStatus(const std::string& str) {
    if (str == "pending") {  // Changed from "Pending"
        return OrderItemStatus::PENDING;
    }
    if (str == "confirmed") {  // Changed from "Confirmed"
        return OrderItemStatus::CONFIRMED;
    }
    if (str == "shipped") {  // Changed from "Shipped"
        return OrderItemStatus::SHIPPED;
    }
    if (str == "delivered") {  // Changed from "Delivered"
        return OrderItemStatus::DELIVERED;
    }
    if (str == "cancelled") {  // Changed from "Cancelled"
        return OrderItemStatus::CANCELLED;
    }
    return OrderItemStatus::PENDING;
}

#endif
