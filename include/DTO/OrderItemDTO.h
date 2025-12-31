#ifndef ORDERITEMDTO_H
#define ORDERITEMDTO_H

#include <string>

#include "../enums/OrderItemStatus.h"

class OrderItemDTO {
   private:
    int _id;  // Database ID
    std::string _productId;
    std::string _productName;
    std::string _sellerId;
    std::string _sellerName;
    double _price;
    int _quantity;
    OrderItemStatus _status;

   public:
    // Updated constructor
    OrderItemDTO(const std::string& productId, const std::string& productName,
                 const std::string& sellerId, const std::string& sellerName, double price, int qty)
        : _id(-1),  // -1 means not yet saved to database
          _productId(productId),
          _productName(productName),
          _sellerId(sellerId),
          _sellerName(sellerName),
          _price(price),
          _quantity(qty),
          _status(OrderItemStatus::PENDING) {
        // Do nothing
    }

    // Getters
    int getId() const {
        return _id;
    }
    std::string getProductId() const {
        return _productId;
    }
    std::string getProductName() const {
        return _productName;
    }
    std::string getSellerId() const {
        return _sellerId;
    }
    std::string getSellerName() const {
        return _sellerName;
    }
    double getPrice() const {
        return _price;
    }
    int getQuantity() const {
        return _quantity;
    }
    OrderItemStatus getStatus() const {
        return _status;
    }

    // Setters
    void setId(int id) {
        _id = id;
    }
    void setStatus(OrderItemStatus status) {
        _status = status;
    }

    // Calculate subtotal
    double getSubtotal() const {
        return _price * _quantity;
    }
};

#endif  // ORDERITEMDTO_H
