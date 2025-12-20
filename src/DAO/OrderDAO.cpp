#include "../../include/DAO/OrderDAO.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"

std::expected<void, std::string> OrderDAO::addOrder(const OrderDTO& order) {
    _orders.push_back(order);
    return {};  // success
}

std::shared_ptr<OrderDTO> OrderDAO::getOrderById(const std::string& id) {
    auto it = std::find_if(_orders.begin(), _orders.end(),
                           [&](const OrderDTO& o) { return o.orderId() == id; });

    if (it != _orders.end()) {
        // return a shared_ptr copy of the stored order
        return std::make_shared<OrderDTO>(*it);
    }

    return nullptr;
}

std::vector<std::shared_ptr<OrderDTO>> OrderDAO::getOrdersByBuyerId(const std::string& id) {
    auto result = std::vector<std::shared_ptr<OrderDTO>>();
    for (const auto& o : _orders) {
        if (o.buyerId() == id) {
            result.push_back(std::make_shared<OrderDTO>(o));
        }
    }
    return result;
}

std::vector<std::shared_ptr<OrderDTO>> OrderDAO::getOrdersBySellerId(const std::string& id) {
    auto result = std::vector<std::shared_ptr<OrderDTO>>();

    for (const auto& o : _orders) {
        // if any item in the order belongs to this seller, include the order
        for (const auto& item : o.items()) {
            if (item.getSellerId() == id) {
                result.push_back(std::make_shared<OrderDTO>(o));
                break;
            }
        }
    }

    return result;
}
