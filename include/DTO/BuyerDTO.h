#ifndef BUYER_DTO_H
#define BUYER_DTO_H

#include <memory>

#include "../DTO/UserDTO.h"
#include "../PurchaseHistory.h"
#include "CartDTO.h"

class BuyerDTO : public UserDTO {
   private:
    double _balance;
    CartDTO _cart;
    PurchaseHistory _purchasesHistory;

   public:
    BuyerDTO(const std::string& id, const std::string& name, const std::string& email,
             const std::string& password, double balance = 0.0);

    // ===== BALANCE =====
    double getBalance() const;
    void setBalance(double amount);

    // ===== CART =====
    CartDTO& getCart();
    const CartDTO& getCart() const;

    // ===== PURCHASE HISTORY =====
    PurchaseHistory& getPurchasesHistory();
    const PurchaseHistory& getPurchasesHistory() const;

    ~BuyerDTO() override = default;
};

#endif
