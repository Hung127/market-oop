#ifndef BUYERBUS_H
#define BUYERBUS_H

#include <expected>
#include <string>
#include <vector>
#include "Buyer_DTO.h"
#include "Order.h"
#include "Utils.h"

class BuyerBus {
public:
    static std::expected<void, std::string> 
    addToCart(BuyerDto* buyer, ProductDto* product, int quantity) {
        if (!buyer || !product) return std::unexpected("Du lieu loi");
        
        if (product->getStock() < quantity) {
            return std::unexpected("San pham khong du hang trong kho!");
        }

        buyer->getCart().addProduct(product, quantity);
        return {}; 
    }

    static std::expected<void, std::string> checkout(BuyerDto* buyer) {
        if (!buyer) return std::unexpected("Buyer null");
        CartDto& cart = buyer->getCart();
        double total = cart.getTotalPrice();
        if (cart.getItems().empty()) {
            return std::unexpected("Gio hang dang trong!");
        }
        if (buyer->getBalance() < total) {
            return std::unexpected("So du khong du de thanh toan!");
        }
        for (const auto& item : cart.getItems()) {
            ProductDto* prod = item.first; 
            int qty = item.second;
            if (!prod->reduceStock(qty)) {
                 return std::unexpected("San pham " + prod->getName() + " da het hang trong luc ban dang thao tac!");
            }
        }

        buyer->deductBalance(total);
        cart.clear();
      
        return {};
    }
};

#endif // BUYER_BUS_H
