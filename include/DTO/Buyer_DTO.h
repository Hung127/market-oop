#ifndef BUYER_DTO_H
#define BUYER_DTO_H

#include <memory>

#include "../PurchaseHistory.h"
#include "../User.h"
#include "Cart_DTO.h"

class BuyerDto : public User {
   private:
    double _balance;
    CartDto _cart;
    PurchaseHistory _purchasesHistory;

   public:
    BuyerDto(const std::string& id, const std::string& name, const std::string& email,
             const std::string& password, double balance = 0.0);

    // ===== BALANCE =====
    double getBalance() const;
    void addBalance(double amount);
    bool hasEnoughBalance(double price);

    // ===== CART =====
    CartDto& getCart();
    const CartDto& getCart() const;

    // ===== PURCHASE HISTORY =====
    PurchaseHistory& getPurchasesHistory();
    const PurchaseHistory& getPurchasesHistory() const;
};

#endif
