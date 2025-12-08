#ifndef BUYER_H
#define BUYER_H

#include <expected>
#include <memory>
#include <vector>

#include "Cart.h"
#include "PurchaseHistory.h"
#include "User.h"

class Buyer : public User {
   private:
    double _balance;
    Cart _cart;
    PurchaseHistory _purchasesHistory;

    // Private constructor
    Buyer(const std::string& id, const std::string& name, const std::string& email,
          const std::string& password, double balance = 0.0);

   public:
    static std::expected<std::unique_ptr<Buyer>, std::string>
    create(const std::string& id, const std::string& name, const std::string& email,
           const std::string& password, double balance = 0.0);

    // ========== BALANCE ==========
    double getBalance() const;
    void addBalance(double amount);
    bool hasEnoughBalance(double price);

    // ========== CART MANAGEMENT ==========
    std::expected<void, std::string> addToCart(const std::shared_ptr<Product>& p, int qty = 1);

    /**
     * @brief Xóa sản phẩm khỏi cart
     */
    bool removeFromCart(const std::string& productId);

    /**
     * @brief Giảm số lượng sản phẩm trong cart
     */
    std::expected<void, std::string> reduceCartQuantity(const std::string& productId, int qty = 1);

    /**
     * @brief Xem cart
     */
    void viewCart() const;

    void clearCart();

    // ========== CHECKOUT ==========
    std::expected<void, std::string> checkout();

    // ========== HISTORY ==========
    void viewPurchaseHistory() const;
};

#endif  // BUYER_H
