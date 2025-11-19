#ifndef ORDERITEM_H
#define ORDERITEM_H
#include "Product.h"
class OrderItem {
   private:
    std::shared_ptr<Product> _product;
    int _quantity;

   public:
    OrderItem(std::shared_ptr<Product> p, int qty) : _product(p), _quantity(qty) {}

    std::shared_ptr<Product> product() const {
        return _product;
    }
    int quantity() const {
        return _quantity;
    }
};

#endif  // ORDERITEM_H
