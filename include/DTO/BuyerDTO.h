#ifndef BUYERDTO_H
#define BUYERDTO_H

#include "User_DTO.h"
#include "Cart_DTO.h"
#include "PurchaseHistory.h" 

class BuyerDto : public UserDto {
private:
    double _balance;
    CartDto _cart;
    PurchaseHistory _history;

public:

    BuyerDto(const std::string& id, const std::string& name, const std::string& email, 
             const std::string& password, double balance = 0.0)
        : UserDto(id, name, email, password), _balance(balance) {}

    std::string getRole() const override { return "Buyer"; }
    double getBalance() const { return _balance; }

    void addBalance(double amount) {
        if (amount > 0) _balance += amount;
    }

    bool deductBalance(double amount) {
        if (_balance >= amount) {
            _balance -= amount;
            return true;
        }
        return false;
    }

    CartDto& getCart() { return _cart; }
    const CartDto& getCart() const { return _cart; }

    PurchaseHistory& getHistory() { return _history; }
    const PurchaseHistory& getHistory() const { return _history; }

    void showProfile() const {
        std::cout << "[BUYER] " << _name << " | Email: " << _email 
                  << " | So du: " << std::fixed << std::setprecision(0) << _balance << " VND\n";
    }
};

#endif // BUYER_DTO_H
