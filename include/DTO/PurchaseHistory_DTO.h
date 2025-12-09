#ifndef PURCHASEHISTORY_H
#define PURCHASEHISTORY_H

#include <vector>
#include <iostream>
#include <iomanip>
#include "Order.h"

class PurchaseHistory {
private:
    std::vector<Order> _orders;

public:
    PurchaseHistory() = default;

    void addOrder(const Order& order) {
        _orders.push_back(order);
    }

    const std::vector<Order>& orders() const {
        return _orders;
    }

    void printHistory() const {
        std::cout << "\n=== LICH SU MUA HANG ===\n";
        if (_orders.empty()) {
            std::cout << "(Trong)\n";
            return;
        }
        for (const auto& ord : _orders) {
            std::cout << "Ngay: " << ord.date() 
                      << " | Tong: " << std::fixed << std::setprecision(2) << ord.totalPrice() << " VND\n";
        }
        std::cout << "========================\n";
    }
};

#endif
