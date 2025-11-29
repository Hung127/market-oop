
#include "../../include/DTO/Buyer_DTO.h"

#include <memory>
#include <string>

#include "../../include/DTO/Cart_DTO.h"
#include "../../include/PurchaseHistory.h"
#include "../../include/User.h"

BuyerDto::BuyerDto(const std::string& id, const std::string& name, const std::string& email,
                   const std::string& password, double balance)
    : User(id, name, email, password), _balance(balance) {}

// ===== BALANCE =====
double BuyerDto::getBalance() const {
    return this->_balance;
}

void BuyerDto::setBalance(double amount) {
    this->_balance = amount;
}

// ===== CART =====

CartDto& BuyerDto::getCart() {
    return this->_cart;
}

const CartDto& BuyerDto::getCart() const {
    return this->_cart;
}
// ===== PURCHASE HISTORY =====
PurchaseHistory& BuyerDto::getPurchasesHistory() {
    return _purchasesHistory;
}

const PurchaseHistory& BuyerDto::getPurchasesHistory() const {
    return _purchasesHistory;
}
