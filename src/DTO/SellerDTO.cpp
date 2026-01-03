#include "../../include/DTO/SellerDTO.h"

#include <memory>

#include "../../include/DTO/ProductDTO.h"

SellerDTO::SellerDTO(const std::string& id, const std::string& name, const std::string& email,
                     const std::string& password)
    : UserDTO::UserDTO(id, name, email, password) {
    // Do nothing
}

// Products accessors (read-only view)
const std::vector<std::shared_ptr<ProductDTO>>& SellerDTO::products() const {
    return this->_myProducts;
}

// Lightweight product operations (no heavy validation)
void SellerDTO::addProduct(std::shared_ptr<ProductDTO> product) {
    this->_myProducts.push_back(product);
}

bool SellerDTO::removeProductById(const std::string& productId) {
    int len = static_cast<int>(this->_myProducts.size());

    for (int i = 0; i < len; ++i) {
        if (!(this->_myProducts[i])) {
            continue;
        }

        if (this->_myProducts[i]->getID() == productId) {
            this->_myProducts.erase(this->_myProducts.begin() + i);
            return true;
        }
    }

    return false;
}  // returns true if removed

std::shared_ptr<ProductDTO> SellerDTO::findProductById(const std::string& productId) const {
    int len = static_cast<int>(this->_myProducts.size());

    for (int i = 0; i < len; ++i) {
        if (!(this->_myProducts[i])) {
            continue;
        }

        if (this->_myProducts[i]->getID() == productId) {
            return this->_myProducts[i];
        }
    }

    return nullptr;
}

std::string SellerDTO::getRole() const {
    return std::string("SELLER");
}
