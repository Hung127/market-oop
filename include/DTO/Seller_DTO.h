#ifndef SELLER_DTO_H
#define SELLER_DTO_H

#include <vector>
#include <memory>
#include <algorithm>
#include "User_DTO.h"
#include "Product_DTO.h"

class SellerDto : public UserDto {
private:

    std::vector<std::shared_ptr<ProductDto>> _myProducts;

public:
    SellerDto(const std::string& id, const std::string& name, const std::string& email, 
              const std::string& password)
        : UserDto(id, name, email, password) {}

    std::string getRole() const override { return "Seller"; }

    std::vector<std::shared_ptr<ProductDto>> getMyProducts() const {
        return _myProducts;
    }

    void addProduct(const std::shared_ptr<ProductDto>& product) {
        if (product) {
            _myProducts.push_back(product);
            std::cout << " >> Shop " << _name << " da dang ban: " << product->getName() << "\n";
        }
    }


    bool removeProduct(const std::string& productId) {
        auto it = std::remove_if(_myProducts.begin(), _myProducts.end(),
            [&](const std::shared_ptr<ProductDto>& p) {
                return p->getId() == productId;
            };
        if (it != _myProducts.end()) {
            _myProducts.erase(it, _myProducts.end());
            std::cout << " >> Da xoa san pham ID: " << productId << "\n";
            return true;
        }
        return false;
    }

    void showShopInfo() const {
        std::cout << "[SHOP] " << _name << " | Email: " << _email 
                  << " | So luong san pham: " << _myProducts.size() << "\n";
    }
};

#endif // SELLER_DTO_H
