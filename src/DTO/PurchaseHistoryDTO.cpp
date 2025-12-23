#include "../../include/DTO/PurchaseHistoryDTO.h"

#include <iomanip>
#include <iostream>
#include <memory>

#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"

void PurchaseHistoryDTO::addOrder(const OrderDTO& order) {
    this->_orders.push_back(order);
}

const std::vector<OrderDTO>& PurchaseHistoryDTO::orders() const {
    return this->_orders;
}

void PurchaseHistoryDTO::printHistory() const {
    if (this->_orders.empty()) {
        std::cout << "No orders in purchase history.\n";
        return;
    }

    std::cout << "=== Purchase History ===\n";
    for (size_t i = 0; i < this->_orders.size(); ++i) {
        const OrderDTO& order = this->_orders[i];

        std::cout << "Order #" << (i + 1) << '\n';
        std::cout << "Date: " << order.date() << '\n';

        // Header
        std::cout << std::left << std::setw(30) << "Product" << std::right << std::setw(8) << "Qty"
                  << std::right << std::setw(12) << "Unit" << std::right << std::setw(12)
                  << "Subtotal" << '\n';
        std::cout << std::string(62, '-') << '\n';

        double running = 0.0;
        for (const std::shared_ptr<OrderItemDTO>& item : order.items()) {
            if (!item) {
                continue;
            }
            double price = item->getPrice();
            int qty = item->getQuantity();
            double subtotal = price * qty;
            running += subtotal;

            std::cout << std::left << std::setw(30) << item->getProductName() << std::right
                      << std::setw(8) << qty << std::right << std::setw(12) << std::fixed
                      << std::setprecision(2) << price << std::right << std::setw(12) << std::fixed
                      << std::setprecision(2) << subtotal << '\n';
        }

        std::cout << std::string(62, '-') << '\n';
        std::cout << std::right << std::setw(50) << "Total: " << std::setw(12) << std::fixed
                  << std::setprecision(2) << order.totalPrice() << "\n\n";
    }
}
