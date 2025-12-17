#include "../../include/DTO/BuyerDTO.h"

#include <string>

#include "../../include/DTO/CartDTO.h"
#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"
#include "../../include/DTO/PurchaseHistoryDTO.h"
#include "../../include/DTO/UserDTO.h"

BuyerDTO::BuyerDTO(const std::string& id, const std::string& name, const std::string& email,
                   const std::string& password, double balance)
    : UserDTO(id, name, email, password), _balance(balance) {}

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
const PurchaseHistoryDTO& BuyerDTO::getPurchasesHistory() const {
    return this->_purchasesHistory;
}

PurchaseHistoryDTO& BuyerDTO::getPurchasesHistory() {
    return this->_purchasesHistory;
}

std::string BuyerDTO::getRole() const {
    return std::string("BUYER");
}
