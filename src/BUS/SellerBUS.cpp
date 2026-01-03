#include "../../include/BUS/SellerBUS.h"

#include <algorithm>
#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../../include/DAO/BuyerDAO.h"
#include "../../include/DAO/OrderDAO.h"
#include "../../include/DAO/ProductDAO.h"
#include "../../include/DTO/BuyerDTO.h"
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
    if (id.empty() || name.empty() || price <= 0.0 || stock < 0) {
        return std::unexpected(BusError::ValidationFailed);
    }

    if (findProductInInventory(id)) {
        return std::unexpected(BusError::Conflict);
    }

    auto p = std::make_shared<ProductDTO>(ProductDTO(id, name, price, stock, this->_seller));
    _seller->addProduct(p);
    return p;
}

std::expected<void, BusError> SellerBUS::updateProduct(const std::string& productId,
                                                       const std::string& newName,
                                                       double newPrice) {
    std::shared_ptr<ProductDTO> prod = findProductInInventory(productId);
    if (!prod) {
        return std::unexpected(BusError::NotFound);
    }

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
    return _seller->products();
}

std::expected<int, BusError> SellerBUS::getProductCount() const {
    return static_cast<int>(_seller->products().size());
}

// ========== SEARCH IN INVENTORY ==========

std::vector<std::shared_ptr<ProductDTO>>
SellerBUS::searchMyProductsByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<ProductDTO>> out;
    if (keyword.empty()) {
        return out;
    }

    for (const auto& p : _seller->products()) {
        if (!p)
            continue;
        if (Utils::SearchHelper::containsIgnoreCase(p->getName(), keyword)) {
            out.push_back(p);
        }
    }

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

    for (const auto& p : _seller->products()) {
        if (!p)
            continue;
        double score = Utils::SearchHelper::similarityScore(p->getName(), keyword);
        if (score > bestScore) {
            bestScore = score;
            best = p;
        }
    }

    constexpr double MIN_ACCEPTABLE_SCORE = 0.25;
    if (best && bestScore >= MIN_ACCEPTABLE_SCORE) {
        return best;
    }

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

    if (item->getStatus() != OrderItemStatus::PENDING) {
        return std::unexpected(BusError::ValidationFailed);
    }

    item->setStatus(OrderItemStatus::CONFIRMED);

    // Save to database
    if (!OrderDAO::updateOrderItem(*item)) {
        return std::unexpected(BusError::InternalError);
    }

    return {};
}

std::expected<void, BusError> SellerBUS::shipOrderItem(const std::string& orderId,
                                                       const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }
    std::shared_ptr<OrderItemDTO> item = itemPack.value();

    if (item->getStatus() != OrderItemStatus::CONFIRMED) {
        return std::unexpected(BusError::ValidationFailed);
    }

    item->setStatus(OrderItemStatus::SHIPPED);

    if (!OrderDAO::updateOrderItem(*item)) {
        return std::unexpected(BusError::InternalError);
    }

    return {};
}

std::expected<void, BusError> SellerBUS::deliverOrderItem(const std::string& orderId,
                                                          const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }
    std::shared_ptr<OrderItemDTO> item = itemPack.value();

    if (item->getStatus() != OrderItemStatus::SHIPPED) {
        return std::unexpected(BusError::ValidationFailed);
    }

    item->setStatus(OrderItemStatus::DELIVERED);

    if (!OrderDAO::updateOrderItem(*item)) {
        return std::unexpected(BusError::InternalError);
    }

    return {};
}

std::expected<void, BusError> SellerBUS::cancelOrderItem(const std::string& orderId,
                                                         const std::string& productId) {
    auto itemPack = findSellerOrderedProductById(orderId, productId);
    if (!itemPack.has_value()) {
        return std::unexpected(itemPack.error());
    }

    std::shared_ptr<OrderItemDTO> item = itemPack.value();

    // Can only cancel pending or confirmed items
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
        // Save product stock update to database
        if (!ProductDAO::update(*product)) {
            return std::unexpected(BusError::InternalError);
        }
    }

    auto order = OrderDAO::getOrderById(orderId);
    if (!order) {
        return std::unexpected(BusError::NotFound);
    }

    // 3. REFUND:  Find buyer and return money
    const std::string& buyerId = order->buyerId();
    auto buyer = BuyerDAO::getBuyerById(buyerId);
    if (buyer) {
        buyer->setBalance(buyer->getBalance() + item->getSubtotal());

        if (!BuyerDAO::save(*buyer)) {
            return std::unexpected(BusError::InternalError);
        }
    }

    // 4. Save order item status to database
    if (!OrderDAO::updateOrderItem(*item)) {
        return std::unexpected(BusError::InternalError);
    }

    // 5. Recalculate and update order total in database
    order->recalculateTotal();
    auto updateResult = OrderDAO::updateOrder(*order);
    if (!updateResult.has_value()) {
        return std::unexpected(BusError::InternalError);
    }

    return {};
}
