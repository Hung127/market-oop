#ifndef SELLERBUS_H
#define SELLERBUS_H

#include <expected>
#include <string>
#include "Seller_DTO.h"

class SellerBus {
public:
    static std::expected<void, std::string> 
    postProduct(SellerDto* seller, const std::string& name, double price, int stock) {
        if (!seller) return std::unexpected("Seller null");
        if (price <= 0) return std::unexpected("Gia phai > 0");
        if (stock < 0) return std::unexpected("So luong khong duoc am");
        std::string prodId = "P_" + Utils::generateId();
        ProductDto* newProd = new ProductDto(prodId, name, price, stock);
        newProd->setOwner(seller);
        seller->addProduct(newProd);
        return {};
    }
};

#endif // SELLER_BUS_H
