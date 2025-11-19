#ifndef PURCHASEHISTORY_H
#define PURCHASEHISTORY_H

#include "Order.h"

class PurchaseHistory {
   private:
    std::vector<Order> _orders;

   public:
    void addOrder(const Order& order) {
        _orders.push_back(order);
    }
    const std::vector<Order>& orders() const {
        return _orders;
    }

   public:
    void printHistory() const;  // Hàm in ra lịch sử đây nha
};
#endif  // PURCHASEHISTORY_H
