#include "../../include/DTO/ProductDTO.h"

#include <memory>

#include "../../include/DTO/SellerDTO.h"

ProductDTO::ProductDTO(const std::string& id, const std::string& name, double price, int stock,
                       const std::shared_ptr<SellerDTO>& owner, const std::string& description,
                       const std::string& imagePath)
    : _owner(owner),
      _sellerId(owner ? owner->getId() : ""),
      _id(id),
      _description(description),
      _name(name),
      _price(price),
      _stock(stock),
      _imagePath(imagePath) {}

// ===== GETTERS (non-const) =====
std::string ProductDTO::getName() {
    return _name;
}
std::string ProductDTO::getID() {
    return _id;
}
double ProductDTO::getPrice() {
    return _price;
}
int ProductDTO::getStock() {
    return _stock;
}
std::shared_ptr<SellerDTO> ProductDTO::getOwner() {
    return _owner.lock();
}

// ===== GETTERS (const) =====
std::string ProductDTO::getName() const {
    return _name;
}
std::string ProductDTO::getID() const {
    return _id;
}
double ProductDTO::getPrice() const {
    return _price;
}
int ProductDTO::getStock() const {
    return _stock;
}
std::shared_ptr<SellerDTO> ProductDTO::getOwner() const {
    return _owner.lock();
}
std::string ProductDTO::getSellerId() const {
    return _sellerId;
}
const std::string& ProductDTO::getImagePath() const {
    return _imagePath;
}
const std::string& ProductDTO::getDescription() const {
    return _description;
}

// ===== SETTERS =====
void ProductDTO::setName(const std::string& name) {
    _name = name;
}
void ProductDTO::setPrice(double price) {
    if (price >= 0)
        _price = price;
}
void ProductDTO::setStock(int stock) {
    if (stock >= 0)
        _stock = stock;
}
void ProductDTO::setOwner(const std::shared_ptr<SellerDTO>& owner) {
    _owner = owner;
    _sellerId = owner ? owner->getId() : "";
}

void ProductDTO::setImagePath(const std::string& path) {
    _imagePath = path;
}

void ProductDTO::setDescription(const std::string& desc) {
    _description = desc;
}
