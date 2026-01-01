#include "../../include/DTO/OrderDTO.h"

#include <memory>

#include "../../include/DTO/OrderItemDTO.h"

OrderDTO::OrderDTO() : _totalPrice(0.0), _rawTotal(0.0), _voucherDiscount(0.0), _coinDiscount(0.0) {}
OrderDTO::OrderDTO(const std::string& orderId, const std::string& buyerId,
                   const std::vector<std::shared_ptr<OrderItemDTO>>& items, double rawTotal,
                   const std::string& date)
    : _orderId(orderId),
      _buyerId(buyerId),
      _items(items),
      _date(date),
      _rawTotal(rawTotal),
      _voucherDiscount(0.0),
      _coinDiscount(0.0),
      _totalPrice(rawTotal) {}
// Accessors
const std::vector<std::shared_ptr<OrderItemDTO>>& OrderDTO::items() const {
    return this->_items;
}

const std::string& OrderDTO::orderId() const {
    return _orderId;
}

const std::string& OrderDTO::buyerId() const {
    return _buyerId;
}

double OrderDTO::totalPrice() const {
    return this->_totalPrice;
}

const std::string& OrderDTO::date() const {
    return this->_date;
}

// Mutators / helpers
void OrderDTO::setItems(const std::vector<std::shared_ptr<OrderItemDTO>>& items) {
    this->_items = items;
}

void OrderDTO::setTotalPrice(double total) {
    this->_totalPrice = total;
}

void OrderDTO::setDate(const std::string& date) {
    this->_date = date;
}

// Convenience
void OrderDTO::addItem(const OrderItemDTO& item) {
    this->_items.push_back(std::make_shared<OrderItemDTO>(item));
}

// Recalculate total from items
void OrderDTO::recalculateTotal() {
    this->_totalPrice = 0.0;

    for (const auto& it : this->_items) {
        if (!it) {
            continue;
        }
        if (it->getStatus() != OrderItemStatus::CANCELLED) {
            this->_totalPrice += it->getPrice() * static_cast<double>(it->getQuantity());
        }
    }
}


void OrderDTO::setDiscounts(double vDiscount, double cDiscount) {
        _voucherDiscount = vDiscount;
        _coinDiscount = cDiscount;
    }
    double OrderDTO::getRawTotal() const { 
        return _rawTotal; 
    }
    double OrderDTO::getVoucherDiscount() const { 
        return _voucherDiscount; 
    }
    double OrderDTO::getCoinDiscount() const { 
        return _coinDiscount; 
    }
