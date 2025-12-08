
#include "../../include/DTO/ProductDTO.h"

#include "../../include/Seller.h"

ProductDto::ProductDto(const std::string& id, const std::string& name, double price, int stock,
                       const std::shared_ptr<Seller>& owner)
    : _id(id), _name(name), _price(price), _stock(stock), _owner(owner) {}

// ========== GETTERS (Non-const) ==========
std::string ProductDto::getName() {
    return this->_name;
}

std::string ProductDto::getID() {
    return this->_id;
}

double ProductDto::getPrice() {
    return this->_price;
}

int ProductDto::getStock() {
    return this->_stock;
}

std::shared_ptr<Seller> ProductDto::getOwner() {
    // Chuyển weak_ptr thành shared_ptr. Trả về nullptr nếu Seller không còn tồn tại.
    return _owner.lock();
}

// ========== GETTERS (Const) ==========
std::string ProductDto::getName() const {
    return this->_name;
}

std::string ProductDto::getID() const {
    return this->_id;
}

double ProductDto::getPrice() const {
    return this->_price;
}

int ProductDto::getStock() const {
    return this->_stock;
}

std::shared_ptr<Seller> ProductDto::getOwner() const {
    return _owner.lock();
}

// ========== SETTERS ==========
void ProductDto::setName(const std::string& name) {
    this->_name = name;
}

void ProductDto::setPrice(double price) {
    if (price >= 0) {
        this->_price = price;
    }
}

void ProductDto::setStock(int stock) {
    if (stock >= 0) {
        this->_stock = stock;
    }
}

void ProductDto::setOwner(const std::shared_ptr<Seller>& owner) {
    this->_owner = owner;
}
