#include "../../include/DAO/OrderDAO.h"

#include <memory>
#include <vector>
#include <expected>

#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"

std::expected<void, std::string> OrderDAO::addOrder(const OrderDTO& order) {
    for (const auto& o : _orders) {
        if (o && o->orderId() == order.orderId()) {
            return std::unexpected("Order ID already exists: " + order.orderId());
        }
    }

    _orders.push_back(std::make_shared<OrderDTO>(order));
    return {};  // success
}

std::shared_ptr<OrderDTO> OrderDAO::getOrderById(const std::string& id) {
    for (std::shared_ptr<OrderDTO> order : _orders) {
        if (!order) {
            continue;
        }
        if (order->orderId() == id) {
            return order;
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<OrderDTO>> OrderDAO::getOrdersByBuyerId(const std::string& id) {
    auto result = std::vector<std::shared_ptr<OrderDTO>>();
    for (const auto& o : _orders) {
        if (!o) {
            continue;
        }
        if (o->buyerId() == id) {
            result.push_back(o);
        }
    }
    return result;
}

std::vector<std::shared_ptr<OrderDTO>> OrderDAO::getOrdersBySellerId(const std::string& id) {
    auto result = std::vector<std::shared_ptr<OrderDTO>>();

    for (const auto& o : _orders) {
        // if any item in the order belongs to this seller, include the order
        for (const auto& item : o->items()) {
            if (!item) {
                continue;
            }
            if (item->getSellerId() == id) {
                result.push_back(o);
                break;
            }
        }
    }

    return result;
}

std::vector<OrderItemDTO> OrderDAO::getSellerOrderItemByStatus(const std::string& sellerId,
                                                               OrderItemStatus status) {
    std::vector<OrderItemDTO> result;

    auto orders = OrderDAO::getOrdersBySellerId(sellerId);

    for (const auto& orderPtr : orders) {
        for (const auto& item : orderPtr->items()) {
            // clang-format off
            if (!item) {
                continue;
            }
            if ((item->getSellerId() == sellerId)
                && (item->getStatus() == status)
            ) {
                result.push_back(*item);
            }
            // clang-format on
        }
    }

    return result;
}
