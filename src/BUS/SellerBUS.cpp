#include "../../include/BUS/SellerBUS.h"

#include <algorithm>
#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../../include/DAO/BuyerDAO.h"
#include "../../include/DAO/OrderDAO.h"
#include "../../include/DTO/ProductDTO.h"
#include "../../include/DTO/SellerDTO.h"
#include "../../include/Utils/Utils.h"

// Helper
std::shared_ptr<ProductDTO> SellerBUS::findProductInInventory(const std::string& id) const {
    return this->_seller->findProductById(id);
}

// ========== PRODUCT MANAGEMENT ==========

std::expected<std::shared_ptr<ProductDTO>, BusError>
SellerBUS::createProduct(const std::string& id, const std::string& name, double price, int stock) {
    // clang-format off
    if (
        (id.empty())
        || (name.empty())
        || (price <= 0.0)
        || (stock < 0)
    ) {
        // clang-format on
        return std::unexpected(BusError::ValidationFailed);
    }

    if (findProductInInventory(id)) {
        return std::unexpected(BusError::Conflict);
    }

    auto p = std::make_shared<ProductDTO>(ProductDTO(id, name, price, stock, this->_seller));
    _seller->addProduct(p);
    return p;
}

// std::expected<void, BusError> SellerBUS::deleteProduct(Market& market,
//                                                        const std::string& productId) {
//     if (productId.empty()) {
//         return std::unexpected(BusError::ValidationFailed);
//     }
//
//     std::shared_ptr<ProductDTO> prod = findProductInInventory(productId);
//     if (!prod) {
//         return std::unexpected(BusError::NotFound);
//     }
//
//     auto listed = market.findProductById(productId);
//     if (listed) {
//         std::expected<void, BusError> unlistPack = market.unlistProduct(productId);
//         if (!unlistPack.has_value()) {
//             // Propagate market error from Market directly
//             return std::unexpected(unlistPack.error());
//         }
//     }
//
//     bool removed = _seller->removeProductById(productId);
//     if (!removed) {
//         return std::unexpected(BusError::InternalError);
//     }
//
//     return {};
// }

std::expected<void, BusError> SellerBUS::updateProduct(const std::string& productId,
                                                       const std::string& newName,
                                                       double newPrice) {
    std::shared_ptr<ProductDTO> prod = findProductInInventory(productId);
    if (!prod) {
        return std::unexpected(BusError::NotFound);
    }

    // Support partial updates:
    // - newName empty => no change to name
    // - newPrice < 0   => no change to price
    // Nothing to update => validation failed
    if (newName.empty() && newPrice < 0) {
        return std::unexpected(BusError::ValidationFailed);
    }

    if (!newName.empty()) {
        prod->setName(newName);
    }
    if (newPrice >= 0) {
        if (newPrice <= 0.0) {
            return std::unexpected(BusError::ValidationFailed);
        }
        prod->setPrice(newPrice);
    }

    return {};
}

std::expected<void, BusError> SellerBUS::updateStock(const std::string& productId, int newStock) {
    if (newStock < 0) {
        return std::unexpected(BusError::ValidationFailed);
    }

    std::shared_ptr<ProductDTO> prod = findProductInInventory(productId);
    if (!prod) {
        return std::unexpected(BusError::NotFound);
    }

    prod->setStock(newStock);
    return {};
}

std::expected<std::vector<std::shared_ptr<ProductDTO>>, BusError> SellerBUS::getMyProducts() const {
    return _seller->products();  // copy of vector of shared_ptrs
}

std::expected<int, BusError> SellerBUS::getProductCount() const {
    return static_cast<int>(_seller->products().size());
}

// ========== MARKET INTERACTION ==========
// TODO: implement market

// std::expected<void, BusError> SellerBUS::publishToMarket(Market& market,
//                                                          const std::string& productId) {
//     std::shared_ptr<ProductDTO> prod = findProductInInventory(productId);
//     if (!prod) {
//         return std::unexpected(BusError::NotFound);
//     }
//
//     // Market API returns std::expected<void, BusError>
//     std::expected<void, BusError> listPack = market.listProduct(prod);
//     if (!listPack.has_value()) {
//         return std::unexpected(listPack.error());
//     }
//
//     return {};
// }

// std::expected<void, BusError> SellerBUS::unpublishFromMarket(Market& market,
//                                                              const std::string& productId) {
//     // clang-format off
//     if (productId.empty()) {
//         return std::unexpected(BusError::ValidationFailed);
//     }
//     // clang-format on
//
//     std::shared_ptr<ProductDTO> prod = findProductInInventory(productId);
//     if (!prod) {
//         return std::unexpected(BusError::NotFound);
//     }
//
//     std::expected<void, BusError> res = market.unlistProduct(productId);
//     if (!res.has_value()) {
//         return std::unexpected(res.error());
//     }
//
//     return {};
// }

// ========== SEARCH IN INVENTORY ==========

std::vector<std::shared_ptr<ProductDTO>>
SellerBUS::searchMyProductsByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<ProductDTO>> out;
    if (keyword.empty()) {
        return out;
    }

    // Collect substring matches (case-insensitive)
    for (const auto& p : _seller->products()) {
        if (!p) {
            continue;
        }
        if (Utils::SearchHelper::containsIgnoreCase(p->getName(), keyword)) {
            out.push_back(p);
        }
    }

    // Compute similarity scores once to avoid repeated expensive computations
    std::vector<std::pair<std::shared_ptr<ProductDTO>, double>> scored;
    scored.reserve(out.size());
    for (const auto& p : out) {
        scored.emplace_back(p, Utils::SearchHelper::similarityScore(p->getName(), keyword));
    }

    std::sort(scored.begin(), scored.end(), [](const auto& a, const auto& b) {
        if (a.second == b.second) {
            return a.first->getName().size() < b.first->getName().size();
        }
        return a.second > b.second;
    });

    // Unpack back to out
    for (size_t i = 0; i < scored.size(); ++i) {
        out[i] = std::move(scored[i].first);
    }
    out.resize(scored.size());

    return out;
}

std::shared_ptr<ProductDTO> SellerBUS::searchMyClosestProduct(const std::string& keyword) const {
    if (keyword.empty()) {
        return nullptr;
    }

    std::shared_ptr<ProductDTO> best = nullptr;
    double bestScore = -1.0;

    // Use similarity score (based on edit distance) to pick the closest match.
    for (const auto& p : _seller->products()) {
        if (!p) {
            continue;
        }
        double score = Utils::SearchHelper::similarityScore(p->getName(), keyword);
        if (score > bestScore) {
            bestScore = score;
            best = p;
        }
    }

    constexpr double MIN_ACCEPTABLE_SCORE = 0.25;
    // clang-format off
    if (
        (best)
        && (bestScore >= MIN_ACCEPTABLE_SCORE)
    ) {
        return best;
    }
    // clang-format on

    return nullptr;
}

std::expected<SellerBUS, BusError> SellerBUS::create(std::shared_ptr<SellerDTO> seller) {
    if (!seller) {
        return std::unexpected(BusError::ValidationFailed);
    }

    return SellerBUS(seller);
}

std::expected<std::shared_ptr<OrderItemDTO>, BusError>
SellerBUS::findSellerOrderedProductById(const std::string& orderId, const std::string& productId) {
    auto order = OrderDAO::getOrderById(orderId);
    if (!order) {
        return std::unexpected(BusError::NotFound);
    }

    auto item = order->findItemByProductId(productId);
    if (!item) {
        return std::unexpected(BusError::NotFound);
    }

    // Verify this item belongs to this seller
    if (item->getSellerId() != _seller->getId()) {
        return std::unexpected(BusError::ValidationFailed);
    }

    return item;
}

std::expected<std::vector<std::shared_ptr<OrderDTO>>, BusError>
SellerBUS::getReceivedOrders() const {
    return OrderDAO::getOrdersBySellerId(_seller->getId());
}

std::expected<std::vector<OrderItemDTO>, BusError>
SellerBUS::getOrderItemsByStatus(OrderItemStatus status) const {
    return OrderDAO::getSellerOrderItemByStatus(_seller->getId(), status);
}

std::expected<void, BusError> SellerBUS::confirmOrderItem(const std::string& orderId,
                                                          const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }
    std::shared_ptr<OrderItemDTO> item = itemPack.value();
    // Can only confirm pending items
    if (item->getStatus() != OrderItemStatus::PENDING) {
        return std::unexpected(BusError::ValidationFailed);
    }

    item->setStatus(OrderItemStatus::CONFIRMED);
    return {};
}

std::expected<void, BusError> SellerBUS::shipOrderItem(const std::string& orderId,
                                                       const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }
    std::shared_ptr<OrderItemDTO> item = itemPack.value();
    // Can only ship confirmed items
    if (item->getStatus() != OrderItemStatus::CONFIRMED) {
        return std::unexpected(BusError::ValidationFailed);
    }

    item->setStatus(OrderItemStatus::SHIPPED);
    return {};
}

std::expected<void, BusError> SellerBUS::deliverOrderItem(const std::string& orderId,
                                                          const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }
    std::shared_ptr<OrderItemDTO> item = itemPack.value();
    // Can only deliver shipped items
    if (item->getStatus() != OrderItemStatus::SHIPPED) {
        return std::unexpected(BusError::ValidationFailed);
    }

    item->setStatus(OrderItemStatus::DELIVERED);
    return {};
}

std::expected<void, BusError> SellerBUS::cancelOrderItem(const std::string& orderId,
                                                         const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }

    std::shared_ptr<OrderItemDTO> item = itemPack.value();
    if (item->getStatus() != OrderItemStatus::PENDING &&
        item->getStatus() != OrderItemStatus::CONFIRMED) {
        return std::unexpected(BusError::ValidationFailed);
    }

    // 1. Update status
    item->setStatus(OrderItemStatus::CANCELLED);

    // 2. Restore stock
    auto product = _seller->findProductById(productId);
    if (product) {
        product->setStock(product->getStock() + item->getQuantity());
    }

    auto order = OrderDAO::getOrderById(orderId);
    if (!order) {
        return std::unexpected(BusError::NotFound);
    }

    // 3. REFUND: Find buyer and return money
    const std::string& buyerId = order->buyerId();
    auto buyer = BuyerDAO::getBuyerById(buyerId);
    if (buyer) {
        // Refund the subtotal of this item only (price * quantity)
        buyer->setBalance(buyer->getBalance() + item->getSubtotal());
        BuyerDAO::save(*buyer);  // Save refund
    }
    // Note: If your order is already updated in memory (since everything is by reference),
    // no need to call an update/save for the order.

    // 4. Optionally, recalculate and update the order's total
    order->recalculateTotal();
    // If you persist purchase history or order record for the buyer, update that too

    return {};
}
