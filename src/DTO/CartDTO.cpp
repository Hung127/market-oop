#include "../../include/DTO/CartDTO.h"

#include "../../include/DTO/ProductDTO.h"

CartDto::CartDto() : _totalPrice(0.0) {}

CartDto::CartDto(const std::vector<std::tuple<std::weak_ptr<ProductDto>, int>>& items)
    : _items(items), _totalPrice(0.0) {}

// ====== GETTERS / SETTERS ======
double CartDto::getTotalPrice() const {
    return _totalPrice;
}

void CartDto::setTotalPrice(double total) {
    this->_totalPrice = total;
}

std::vector<std::tuple<std::weak_ptr<ProductDto>, int>>& CartDto::getItems() {
    return _items;
}

const std::vector<std::tuple<std::weak_ptr<ProductDto>, int>>& CartDto::getItems() const {
    return _items;
}
