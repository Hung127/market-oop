
#include "../../include/DTO/BuyerDTO.h"

#include <memory>
#include <string>

#include "../../include/DTO/CartDTO.h"
#include "../../include/PurchaseHistory.h"
#include "../../include/User.h"

BuyerDTO::BuyerDTO(const std::string& id, const std::string& name, const std::string& email,
                   const std::string& password, double balance)
    : User(id, name, email, password), _balance(balance) {}

// ===== BALANCE =====
double BuyerDTO::getBalance() const {
    return this->_balance;
}

void BuyerDTO::setBalance(double amount) {
    this->_balance = amount;
}

// ===== CART =====

CartDTO& BuyerDTO::getCart() {
    return this->_cart;
}

const CartDTO& BuyerDTO::getCart() const {
    return this->_cart;
}
// ===== PURCHASE HISTORY =====
PurchaseHistory& BuyerDTO::getPurchasesHistory() {
    return this->_purchasesHistory;
}

const PurchaseHistory& BuyerDTO::getPurchasesHistory() const {
    return this->_purchasesHistory;
}
