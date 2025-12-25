#include "../../include/DTO/OrderDTO.h"

#include "../../include/DTO/OrderItemDTO.h"

/*
  Lưu ý:
  - Đoạn recalculateTotal() dưới đây giả định OrderItemDTO có các phương thức:
      double getPrice() const;
      int getQuantity() const;
    Nếu tên phương thức trong OrderItemDTO của bạn khác, hãy thay các gọi tương ứng.
*/

OrderDTO::OrderDTO() : _items(), _totalPrice(0.0), _date() {
    // khởi tạo đã thực hiện trong initializer list
}

OrderDTO::OrderDTO(const std::vector<OrderItemDTO>& items, double rawTotal, const std::string& date)
        : _items(items), _rawTotal(rawTotal), _date(date), 
          _voucherDiscount(0), _coinDiscount(0), _totalPrice(rawTotal) {}

// Accessors
const std::vector<OrderItemDTO>& OrderDTO::items() const {
    return this->_items;
}

double OrderDTO::totalPrice() const {
    return this->_totalPrice;
}

const std::string& OrderDTO::date() const {
    return this->_date;
}

// Mutators / helpers
void OrderDTO::setItems(const std::vector<OrderItemDTO>& items) {
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
    this->_items.push_back(item);
}

// Recalculate total from items
void OrderDTO::recalculateTotal() {
    this->_totalPrice = 0.0;

    // Giả sử OrderItemDTO có getPrice() và getQuantity()
    for (const auto& it : this->_items) {
        this->_totalPrice += it.getPrice() * static_cast<double>(it.getQuantity());
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
