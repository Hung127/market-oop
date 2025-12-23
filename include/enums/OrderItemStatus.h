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

// Helper function to convert enum to string
inline std::string orderItemStatusToString(OrderItemStatus status) {
    switch (status) {
        case OrderItemStatus::PENDING:
            return "Pending";
        case OrderItemStatus::CONFIRMED:
            return "Confirmed";
        case OrderItemStatus::SHIPPED:
            return "Shipped";
        case OrderItemStatus::DELIVERED:
            return "Delivered";
        case OrderItemStatus::CANCELLED:
            return "Cancelled";
        default:
            return "Unknown";
    }
}

// Helper to parse string to enum
inline OrderItemStatus stringToOrderItemStatus(const std::string& str) {
    if (str == "Pending") {
        return OrderItemStatus::PENDING;
    }
    if (str == "Confirmed") {
        return OrderItemStatus::CONFIRMED;
    }
    if (str == "Shipped") {
        return OrderItemStatus::SHIPPED;
    }
    if (str == "Delivered") {
        return OrderItemStatus::DELIVERED;
    }
    if (str == "Cancelled") {
        return OrderItemStatus::CANCELLED;
    }
    return OrderItemStatus::PENDING;
}

#endif  // ORDERITEMSTATUSDTO_H
