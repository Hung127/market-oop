#ifndef CARTDTO_H
#define CARTDTO_H

#include <vector>
#include <iostream>
#include <algorithm>
#include "Product_DTO.h"

class CartDto {
private:
    std::vector<std::pair<ProductDto*, int>> _items;

public:
    CartDto() = default;

    void addProduct(ProductDto* product, int quantity = 1) {
        if (product == nullptr) return;
      
        for (auto& item : _items) {
            if (item.first->getId() == product->getId()) {
                item.second += quantity; // Cộng dồn
                std::cout << " >> Da cap nhat so luong " << product->getName() << "\n";
                return;
            }
        }

        // Chưa có thì thêm mới
        _items.push_back({product, quantity});
        std::cout << " >> Da them " << product->getName() << " vao gio.\n";
    }

    void removeProduct(const std::string& productId) {
        auto it = std::remove_if(_items.begin(), _items.end(),
            [&](const std::pair<ProductDto*, int>& pair) {
                return pair.first->getId() == productId;
            });

        if (it != _items.end()) {
            _items.erase(it, _items.end());
        }
    }

    double getTotalPrice() const {
        double total = 0;
        for (const auto& item : _items) {
            total += item.first->getPrice() * item.second;
        }
        return total;
    }

    void clear() {
        _items.clear();
    }

    const std::vector<std::pair<ProductDto*, int>>& getItems() const {
        return _items;
    }

    void display() const {
        std::cout << "\n--- GIO HANG ---\n";
        if (_items.empty()) {
            std::cout << "(Trong)\n"; 
            return; 
        }

        for (const auto& item : _items) {
            std::cout << "- " << item.first->getName() 
                      << " | SL: " << item.second 
                      << " | Thanh tien: " << (long long)(item.first->getPrice() * item.second) << "\n";
        }
        std::cout << "TONG: " << (long long)getTotalPrice() << " VND\n";
    }
};

#endif
