#include "../../include/dto/MarketDTO.h"

#include <memory>

bool MarketDTO::addProduct(const std::shared_ptr<Product>& product) {
    if (!product) {
        return false;
    }

    std::string key = product->getID();
    auto it = _listedProducts.find(key);

    if (it != _listedProducts.end()) {
        if (!it->second.expired()) {
            return false;
        }
        it->second = std::weak_ptr<Product>(product);
        return true;
    }

    std::weak_ptr<Product> p(product);
    this->_listedProducts.emplace(key, p);

    return true;
}

bool MarketDTO::removeProduct(const std::string& id) {
    if (this->_listedProducts.contains(id)) {
        this->_listedProducts.erase(id);
        return true;
    }
    return false;
}

std::shared_ptr<Product> MarketDTO::findProduct(const std::string& id) const {
    auto p = this->_listedProducts.find(id);
    if ((p != this->_listedProducts.end())) {
        if (std::shared_ptr<Product> product = p->second.lock()) {
            return product;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Product>> MarketDTO::getAllProducts() const {
    std::vector<std::shared_ptr<Product>> result;
    for (auto it = this->_listedProducts.begin(); it != this->_listedProducts.end();  // NOLINT
         ++it) {
        if (auto p = it->second.lock()) {
            result.push_back(p);
        }
    }
    return result;
}

int MarketDTO::countAllProducts() const {
    int result = 0;
    for (auto it = this->_listedProducts.begin(); it != this->_listedProducts.end();  // NOLINT
         ++it) {
        if (std::shared_ptr<Product> p = it->second.lock()) {
            ++result;
        }
    }
    return result;
}

void MarketDTO::cleanupExpired() {
    for (auto it = _listedProducts.begin(); it != _listedProducts.end();) {
        if (it->second.expired()) {
            it = _listedProducts.erase(it);
        } else {
            ++it;
        }
    }
}
