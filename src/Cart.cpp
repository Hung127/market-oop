#include "../include/Cart.h"

#include <algorithm>
#include <expected>
#include <format>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

std::expected<void, std::string> Cart::add(const std::shared_ptr<Product>& p, int quantity) {
    if (!p) {
        return std::unexpected("Invalid product");
    }

    if (!p->hasEnoughStock(quantity)) {
        return std::unexpected(
            std::format("Not enough stock for '{}'. Available: {}, Requested: {}", p->getName(),
                        p->getStock(), quantity));
    }

    std::string id = p->getID();

    for (auto& item : this->_items) {
        auto& [cartProduct, currentQuantity] = item;

        if (std::shared_ptr<Product> product = cartProduct.lock()) {
            if (product->getID() != id) {
                continue;
            }

            // Update quantity
            int newQuantity = currentQuantity + quantity;

            if (!p->hasEnoughStock(newQuantity)) {
                return std::unexpected(
                    std::format("Not enough stock for '{}'. Available: {}, In cart: {}, "
                                "Trying to add: {}",
                                p->getName(), p->getStock(), currentQuantity, quantity));
            }

            currentQuantity = newQuantity;

            // Recalculate total
            auto recalcResult = recalculateTotal();
            if (!recalcResult.has_value()) {
                return std::unexpected(recalcResult.error());
            }

            return {};
        }
    }

    // Product chưa có → thêm mới
    this->_items.push_back(std::make_tuple(std::weak_ptr<Product>(p), quantity));

    // Recalculate total
    auto recalcResult = recalculateTotal();
    if (!recalcResult.has_value()) {
        return std::unexpected(recalcResult.error());
    }

    return {};
}

std::expected<void, std::string> Cart::recalculateTotal() {
    this->_totalPrice = 0.0;

    auto it = this->_items.begin();
    while (it != this->_items.end()) {
        auto& [weakP, qty] = *it;

        if (auto p = weakP.lock()) {
            if (!p->hasEnoughStock(qty)) {
                return std::unexpected(
                    std::format("Not enough stock for '{}'. Available: {}, In cart: {}",
                                p->getName(), p->getStock(), qty));
            }

            this->_totalPrice += p->getPrice() * qty;
            ++it;
        } else {
            // Xóa weak_ptr đã expired
            it = this->_items.erase(it);
        }
    }

    return {};
}

std::expected<double, std::string> Cart::getTotal() {
    auto totalPack = recalculateTotal();
    if (!totalPack.has_value()) {
        return std::unexpected(totalPack.error());
    }

    return this->_totalPrice;
}

Cart::Cart() : _totalPrice(0.0) {}

void Cart::clear() {
    this->_items.clear();
    this->_totalPrice = 0;
}

Cart::Cart(const std::vector<std::tuple<std::weak_ptr<Product>, int>>& items)
    : _items(items), _totalPrice(0) {
    for (auto& it : this->_items) {
        auto [cartProduct, qty] = it;
        if (std::shared_ptr<Product> product = cartProduct.lock()) {
            this->_totalPrice += product->getPrice() * qty;
        }
    }
}

std::vector<std::tuple<std::shared_ptr<Product>, int>> Cart::getItems() const {
    std::vector<std::tuple<std::shared_ptr<Product>, int>> result;

    for (const auto& [weakP, qty] : _items) {
        if (auto p = weakP.lock()) {
            result.push_back(std::make_tuple(p, qty));
        }
    }

    return result;
}

// ========================================================================
// NEW METHODS ✅
// ========================================================================

bool Cart::removeItem(const std::string& productId) {
    auto it = std::find_if(_items.begin(), _items.end(), [&productId](const auto& item) {
        auto& [weakPtr, qty] = item;
        if (auto product = weakPtr.lock()) {
            return product->getID() == productId;
        }
        return false;
    });

    if (it == _items.end()) {
        return false;  // Không tìm thấy sản phẩm
    }

    // Lấy tên sản phẩm để log
    std::string productName = "Unknown";
    auto& [weakPtr, qty] = *it;
    if (auto product = weakPtr.lock()) {
        productName = product->getName();
    }

    // Xóa khỏi cart
    _items.erase(it);

    // Recalculate total
    auto recalculatePack = recalculateTotal();

    if (!recalculatePack.has_value()) {
        return false;
    }

    std::cout << "[Cart] ✓ Removed: " << productName << " (ID: " << productId << ")\n";

    return true;
}

std::expected<void, std::string> Cart::reduceQuantity(const std::string& productId, int quantity) {
    if (quantity <= 0) {
        return std::unexpected("Quantity to reduce must be positive");
    }

    // Tìm sản phẩm trong cart
    for (auto& item : _items) {
        auto& [weakPtr, currentQty] = item;

        if (auto product = weakPtr.lock()) {
            if (product->getID() != productId) {
                continue;
            }

            // Tìm thấy sản phẩm
            std::string productName = product->getName();

            if (quantity > currentQty) {
                return std::unexpected(
                    std::format("Cannot reduce {} units of '{}'. Only {} in cart.", quantity,
                                productName, currentQty));
            }

            int newQty = currentQty - quantity;

            if (newQty <= 0) {
                // Xóa sản phẩm nếu quantity về 0
                removeItem(productId);
                std::cout << "[Cart] ✓ Reduced '" << productName << "' by " << quantity
                          << " (removed from cart)\n";
            } else {
                // Cập nhật quantity mới
                currentQty = newQty;

                // Recalculate total
                auto recalcResult = recalculateTotal();
                if (!recalcResult.has_value()) {
                    return std::unexpected(recalcResult.error());
                }

                std::cout << "[Cart] ✓ Reduced '" << productName << "' by " << quantity
                          << " (new quantity: " << newQty << ")\n";
            }

            return {};
        }
    }

    return std::unexpected(std::format("Product '{}' not found in cart", productId));
}

int Cart::getQuantity(const std::string& productId) const {
    for (const auto& [weakPtr, qty] : _items) {
        if (auto product = weakPtr.lock()) {
            if (product->getID() == productId) {
                return qty;
            }
        }
    }
    return 0;
}

bool Cart::hasProduct(const std::string& productId) const {
    return getQuantity(productId) > 0;
}

void Cart::displayCart() const {
    if (_items.empty()) {
        std::cout << "\n[Cart] Your cart is empty.\n\n";
        return;
    }

    std::cout << "     CART LIST     \n";

    std::cout << std::left << std::setw(10) << "ID" << std::setw(30) << "Product" << std::right
              << std::setw(12) << "Price" << std::setw(8) << "Qty" << std::setw(15) << "Subtotal"
              << "\n";
    std::cout << std::string(75, '-') << "\n";

    double total = 0.0;

    for (const auto& [weakPtr, qty] : _items) {
        if (auto product = weakPtr.lock()) {
            double subtotal = product->getPrice() * qty;
            total += subtotal;

            std::cout << std::left << std::setw(10) << product->getID() << std::setw(30)
                      << product->getName() << std::right << "$" << std::setw(11) << std::fixed
                      << std::setprecision(2) << product->getPrice() << std::setw(8) << qty << "$"
                      << std::setw(14) << subtotal << "\n";
        }
    }

    std::cout << std::string(75, '-') << "\n";
    std::cout << std::right << std::setw(60) << "TOTAL: "
              << "$" << std::setw(14) << total << "\n\n";
}
