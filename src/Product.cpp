#include "../include/Product.h"

#include <format>
#include <iostream>

#include "../include/Seller.h"

Product::Product(const std::string& id, const std::string& name, double price, int stock,
                 const std::shared_ptr<Seller>& owner)
    : _owner(owner), _id(id), _name(name), _price(price), _stock(stock) {}

// ========== GETTERS (Non-const) ==========
std::string Product::getName() {
    return this->_name;
}
std::string Product::getID() {
    return this->_id;
}
double Product::getPrice() {
    return this->_price;
}
int Product::getStock() {
    return this->_stock;
}
std::shared_ptr<Seller> Product::getOwner() {
    return _owner.lock();
}

// ========== GETTERS (Const) ==========
std::string Product::getName() const {
    return this->_name;
}
std::string Product::getID() const {
    return this->_id;
}
double Product::getPrice() const {
    return this->_price;
}
int Product::getStock() const {
    return this->_stock;
}
std::shared_ptr<Seller> Product::getOwner() const {
    return _owner.lock();
}

// ========== STOCK METHODS ==========
bool Product::hasEnoughStock(int quantity) {
    return (this->_stock >= quantity && quantity > 0);
}

bool Product::hasEnoughStock(int quantity) const {
    return (this->_stock >= quantity && quantity > 0);
}

std::expected<void, std::string> Product::reduceStock(int quantity) {
    if (!hasEnoughStock(quantity)) {
        return std::unexpected(std::format(
            "Do not have enough stock. Available: {}, Need to reduce: {}", this->_stock, quantity));
    }

    this->_stock -= quantity;
    return {};
}

void Product::addStock(int quantity) {
    if (quantity > 0) {
        this->_stock += quantity;
    }
}

// ========== OWNER METHODS ==========
void Product::setOwner(const std::shared_ptr<Seller>& owner) {
    this->_owner = owner;
}

bool Product::hasOwner() const {
    return !_owner.expired();
}

// ========== DISPLAY METHODS ✅ THÊM MỚI ==========

std::string Product::toString() const {
    std::stringstream ss;

    // Format: "ID: P001  | Name: Laptop Dell XPS    | Price: $1299.99 | Stock:
    // 10"
    ss << "ID: " << std::left << std::setw(8) << _id << " | Name: " << std::left << std::setw(30)
       << _name << " | Price: $" << std::fixed << std::setprecision(2) << std::right
       << std::setw(10) << _price << " | Stock: " << std::right << std::setw(5) << _stock;

    // Thêm thông tin seller nếu có
    if (auto seller = _owner.lock()) {
        ss << " | Seller: " << seller->getName();
    }

    return ss.str();
}

void Product::display() const {
    std::cout << toString() << std::endl;
}
