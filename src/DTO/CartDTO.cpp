#include "../../include/DTO/CartDTO.h"

#include "../../include/DTO/ProductDTO.h"

CartDTO::CartDTO() : _totalPrice(0.0) {}

CartDTO::CartDTO(const std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>>& items)
    : _items(items), _totalPrice(0.0) {}

// ====== GETTERS / SETTERS ======
double CartDTO::getTotalPrice() const {
    return _totalPrice;
}

void CartDTO::setTotalPrice(double total) {
    this->_totalPrice = total;
}

std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>>& CartDTO::getItems() {
    return _items;
}

const std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>>& CartDTO::getItems() const {
    return _items;
}
