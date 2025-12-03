#ifndef PRODUCT_DTO_H
#define PRODUCT_DTO_H

#include <string>
#include <iostream>
#include <iomanip>

class SellerDto;

class ProductDto {
private:
    std::string _id;
    std::string _name;
    double _price;
    int _stock;
    
    SellerDto* _owner;
public:
    ProductDto(const std::string& id, const std::string& name, double price, int stock)
        : _id(id), _name(name), _price(price), _stock(stock), _owner(nullptr) {}

    std::string getId() const { return _id; }
    std::string getName() const { return _name; }
    double getPrice() const { return _price; }
    int getStock() const { return _stock; }
    
    void setOwner(SellerDto* owner) {
        _owner = owner;
    }

    bool reduceStock(int quantity) {
        if (quantity > 0 && _stock >= quantity) {
            _stock -= quantity;
            return true;
        }
        return false;
    }

    void display() const {
        std::cout << "| " << std::left << std::setw(6) << _id 
                  << "| " << std::setw(20) << _name 
                  << "| " << std::setw(10) << std::fixed << std::setprecision(0) << _price << " VND"
                  << "| SL: " << _stock << " |\n";
    }
    
    std::string toString() const {
        return _name + " (" + std::to_string((int)_price) + ")";
    }
};

#endif
