
#include "../../include/DTO/ProductDTO.h"

#include "../../include/Seller.h"

ProductDTO::ProductDTO(const std::string& id, const std::string& name, double price, int stock,
                       const std::shared_ptr<Seller>& owner)
    : _id(id), _name(name), _price(price), _stock(stock), _owner(owner) {}

// ========== GETTERS (Non-const) ==========
std::string ProductDTO::getName() {
    return this->_name;
}

std::string ProductDTO::getID() {
    return this->_id;
}

double ProductDTO::getPrice() {
    return this->_price;
}

int ProductDTO::getStock() {
    return this->_stock;
}

std::shared_ptr<Seller> ProductDTO::getOwner() {
    // Chuyển weak_ptr thành shared_ptr. Trả về nullptr nếu Seller không còn tồn tại.
    return this->_owner.lock();
}

// ========== GETTERS (Const) ==========
std::string ProductDTO::getName() const {
    return this->_name;
}

std::string ProductDTO::getID() const {
    return this->_id;
}

double ProductDTO::getPrice() const {
    return this->_price;
}

int ProductDTO::getStock() const {
    return this->_stock;
}

std::shared_ptr<Seller> ProductDTO::getOwner() const {
    return this->_owner.lock();
}

// ========== SETTERS ==========
void ProductDTO::setName(const std::string& name) {
    this->_name = name;
}

void ProductDTO::setPrice(double price) {
    if (price >= 0) {
        this->_price = price;
    }
}

void ProductDTO::setStock(int stock) {
    if (stock >= 0) {
        this->_stock = stock;
    }
}

void ProductDTO::setOwner(const std::shared_ptr<Seller>& owner) {
    this->_owner = owner;
}
