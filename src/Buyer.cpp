#include "Buyer.h"
#include <chrono>
#include <format>
#include <memory>

// Helper function để lấy ngày hiện tại
std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S",
                  std::localtime(&now_time));
    return std::string(buffer);
}

// Private constructor
Buyer::Buyer(const std::string& id, const std::string& name,
             const std::string& email, const std::string& password,
             double balance)
    : User(id, name, email, password), _balance(balance), _cart() {}

std::expected<std::unique_ptr<Buyer>, std::string>
Buyer::create(const std::string& id, const std::string& name,
              const std::string& email, const std::string& password,
              double balance) {
    // check and add id
    for (const std::string& usedId : User::_ids) {
        if (usedId == id) {
            return std::unexpected("User id is already used");
        }
    }

    // add to used id
    User::_ids.insert(id);

    return std::unique_ptr<Buyer>(
        new Buyer(id, name, email, password, balance));
}

// ========================================================================
// BALANCE
// ========================================================================

double Buyer::getBalance() const { return _balance; }

void Buyer::addBalance(double amount) {
    if (amount > 0) {
        _balance += amount;
    }
}

bool Buyer::hasEnoughBalance(double price) { return (this->_balance >= price); }

// ========================================================================
// CART MANAGEMENT
// ========================================================================

std::expected<void, std::string>
Buyer::addToCart(const std::shared_ptr<Product>& p, int qty) {
    return this->_cart.add(p, qty);
}

bool Buyer::removeFromCart(const std::string& productId) {
    return this->_cart.removeItem(productId);
}

std::expected<void, std::string>
Buyer::reduceCartQuantity(const std::string& productId, int qty) {
    return this->_cart.reduceQuantity(productId, qty);
}

void Buyer::viewCart() const { this->_cart.displayCart(); }

void Buyer::clearCart() { this->_cart.clear(); }

// ========================================================================
// CHECKOUT
// ========================================================================

std::expected<void, std::string> Buyer::checkout() {
    // 1. Get total (validates stock)
    auto totalPricePack = this->_cart.getTotal();
    if (!totalPricePack.has_value()) {
        return std::unexpected(totalPricePack.error());
    }
    double totalPrice = totalPricePack.value();

    // 2. Check balance
    if (!hasEnoughBalance(totalPrice)) {
        return std::unexpected(
            std::format("Insufficient balance. Need: ${:.2f}, Have: ${:.2f}",
                        totalPrice, getBalance()));
    }

    // 3. Get items before clearing cart
    auto items = this->_cart.getItems();

    // 4. Reduce stock
    for (const auto& [product, quantity] : items) {
        auto reducePack = product->reduceStock(quantity);
        if (!reducePack.has_value()) {
            return std::unexpected(reducePack.error());
        }
    }

    // 5. Deduct balance
    this->_balance -= totalPrice;

    // 6. CREATE ORDER AND SAVE TO HISTORY
    // std::vector<OrderItem> orderItems;
    // for (const auto& [product, quantity] : items) {
    //    orderItems.push_back(OrderItem(product, quantity));
    //}

    // Order order(orderItems, totalPrice, getCurrentDate());
    // this->_purchasesHistory.addOrder(order);

    // 7. Clear cart
    this->_cart.clear();

    return {};
}

// ========================================================================
// HISTORY
// ========================================================================

void Buyer::viewPurchaseHistory() const {
    this->_purchasesHistory.printHistory();
}
