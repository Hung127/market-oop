#ifndef ORDERITEMDTO_H
#define ORDERITEMDTO_H

#include <string>

class OrderItemDTO {
   private:
    std::string _productId;
    std::string _sellerId;
    std::string _productName;
    double _price;
    int _quantity;

   public:
    OrderItemDTO(const std::string& id, const std::string& sId, const std::string& name, double price, int qty)
        : _productId(id), _sellerId(sId), _productName(name), _price(price), _quantity(qty) {}

    std::string getSellerId() const { 
        return _sellerId; 
    }

    std::string getProductId() const {
        return _productId;
    }
    std::string getProductName() const {
        return _productName;
    }
    double getPrice() const {
        return _price;
    }
    int getQuantity() const {
        return _quantity;
    }

    // Tính tổng tiền của item này
    double getSubtotal() const {
        return _price * _quantity;
    }
};

#endif  // ORDERITEMDTO_H
