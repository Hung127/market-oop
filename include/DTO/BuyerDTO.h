#ifndef BUYER_DTO_H
#define BUYER_DTO_H

#include <memory>

#include "../DTO/CartDTO.h"
#include "../DTO/UserDTO.h"
#include "../PurchaseHistory.h"

class BuyerDTO : public UserDTO {
   private:
    double _balance;
    CartDTO _cart;
    PurchaseHistory _purchasesHistory;

   public:
    BuyerDTO(const std::string& id, const std::string& name, const std::string& email,
             const std::string& password, double balance = 0.0);

    double getBalance() const;
    void setBalance(double amount);

    CartDTO& getCart();
    const CartDTO& getCart() const;

    PurchaseHistory& getPurchasesHistory();
    const PurchaseHistory& getPurchasesHistory() const;
    std::string getRole() const override;

    ~BuyerDTO() override = default;
};

#endif
