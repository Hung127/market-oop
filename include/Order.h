#ifndef ORDER_H
#define ORDER_H
#include <vector>

#include "OrderItem.h"

class Order {
   private:
    std::vector<OrderItem> _items;
    double _totalPrice;
    std::string _date;

   public:
    Order(const std::vector<OrderItem>& items, double total, const std::string& date)
        : _items(items), _totalPrice(total), _date(date) {}

    const std::vector<OrderItem>& items() const {
        return _items;
    }
    double totalPrice() const {
        return _totalPrice;
    }
    std::string date() const {
        return _date;
    }
};

#endif  // ORDER_H
