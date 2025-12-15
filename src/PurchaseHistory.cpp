#include <iomanip>
#include <iostream>

#include "../include/DTO/OrderDTO.h"  // Full definition is needed to inspect contents in printHistory
#include "../include/DTO/OrderItemDTO.h"  // Full definition is needed to inspect contents in printHistory
#include "../include/DTO/PurchaseHistoryDTO.h"

void PurchaseHistory::addOrder(const OrderDTO& order) {
    _orders.push_back(order);
}

const std::vector<OrderDTO>& PurchaseHistory::orders() const {
    return _orders;
}

void PurchaseHistory::printHistory() const {
    if (_orders.empty()) {
        std::cout << "Chưa có đơn hàng nào trong lịch sử mua.\n";
        return;
    }

    std::cout << "===== LỊCH SỬ MUA HÀNG =====\n";

    for (size_t i = 0; i < _orders.size(); i++) {
        const OrderDTO& order = _orders[i];

        std::cout << "Đơn hàng #" << (i + 1) << "\n";
        std::cout << "Ngày mua: " << order.date() << "\n\n";
        std::cout << std::left << std::setw(25) << "Tên sản phẩm" << std::right << std::setw(10)
                  << "SL" << std::right << std::setw(15) << "Đơn giá" << std::right << std::setw(15)
                  << "Thành tiền"
                  << "\n";

        std::cout << std::string(65, '-') << "\n";

        double total = 0;
        for (auto& item : order.items()) {
            double price = item.getPrice();
            int qty = item.getQuantity();
            float amount = price * qty;
            total += amount;

            std::cout << std::left << std::setw(25) << item.getProductName() << std::right
                      << std::setw(10) << qty << std::right << std::setw(15) << price << std::right
                      << std::setw(15) << amount << "\n";
        }

        std::cout << std::string(65, '-') << "\n";
        std::cout << std::right << std::setw(50) << "Tổng thanh toán: " << std::setw(15) << total
                  << "\n\n";
    }
}
