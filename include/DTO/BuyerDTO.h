#ifndef BUYER_DTO_H
#define BUYER_DTO_H

#include <memory>

#include "CartDTO.h"
#include "PurchaseHistoryDTO.h"
#include "UserDTO.h"

class BuyerDTO : public UserDTO {
   private:
    double _balance;
    CartDTO _cart;
    PurchaseHistoryDTO _purchasesHistory;

   public:
    BuyerDTO(const std::string& id, const std::string& name, const std::string& email,
             const std::string& password, double balance = 0.0);
    ~BuyerDTO() override = default;

    double getBalance() const;
    void setBalance(double amount);

    CartDTO& getCart();
    const CartDTO& getCart() const;

    const PurchaseHistoryDTO& getPurchasesHistory() const;
    PurchaseHistoryDTO& getPurchasesHistory();
    std::string getRole() const override;
};

#endif
