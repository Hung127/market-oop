#include "../../include/DTO/PurchaseHistoryDTO.h"

#include <iomanip>
#include <iostream>

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

        // --- GIỮ NGUYÊN PHẦN HEADER CŨ ---
        std::cout << std::left << std::setw(30) << "Product" << std::right << std::setw(8) << "Qty"
                  << std::right << std::setw(12) << "Unit" << std::right << std::setw(12)
                  << "Subtotal" << '\n';
        std::cout << std::string(62, '-') << '\n';

        // --- GIỮ NGUYÊN VÒNG LẶP IN ITEM CŨ ---
        for (const OrderItemDTO& item : order.items()) {
            std::cout << std::left << std::setw(30) << item.getProductName() << std::right
                      << std::setw(8) << item.getQuantity() << std::right << std::setw(12) << std::fixed
                      << std::setprecision(2) << item.getPrice() << std::right << std::setw(12) << std::fixed
                      << std::setprecision(2) << (item.getPrice() * item.getQuantity()) << '\n';
        }

        std::cout << std::string(62, '-') << '\n';

        // --- PHẦN THÊM MỚI: CHI TIẾT GIẢM GIÁ ---
        // Chỉ hiện nếu có giá trị giảm giá để giao diện vẫn sạch sẽ như cũ
        if (order.getVoucherDiscount() > 0) {
            std::cout << std::right << std::setw(50) << "Voucher Discount: " 
                      << "-" << std::setw(11) << order.getVoucherDiscount() << '\n';
        }
        if (order.getCoinDiscount() > 0) {
            std::cout << std::right << std::setw(50) << "Coins Used: " 
                      << "-" << std::setw(11) << order.getCoinDiscount() << '\n';
        }

        // --- GIỮ NGUYÊN DÒNG TOTAL CŨ (Chỉ đổi nhãn thành Final Total cho rõ) ---
        std::cout << std::right << std::setw(50) << "Final Total: " << std::setw(12) << std::fixed
                  << std::setprecision(2) << order.totalPrice() << "\n\n";
    }
}
