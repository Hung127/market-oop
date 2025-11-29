#include "../../include/BUS/Product_BUS.h"

#include <expected>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "../../include/DTO/Product_DTO.h"

std::expected<void, std::string> ProductBus::reduceStock(ProductDto& p, int quantity) {
    if (quantity <= 0)
        return std::unexpected("Quantity must be positive");
    if (p.getStock() < quantity)
        return std::unexpected("Not enough stock");

    p.setStock(p.getStock() - quantity);
    return {};
}

void ProductBus::addStock(ProductDto& p, int quantity) {
    if (quantity > 0) {
        p.setStock(p.getStock() + quantity);
    }
}

bool ProductBus::hasEnoughStock(const ProductDto& p, int quantity) {
    return p.getStock() >= quantity && quantity > 0;
}

bool ProductBus::isInStock(const ProductDto& p) {
    return p.getStock() > 0;
}

// ========== DATA UPDATE LOGIC (Validation) ==========

std::expected<void, std::string> ProductBus::updatePrice(ProductDto& p, double newPrice) {
    if (newPrice <= 0)
        return std::unexpected("Price must be greater than 0");
    p.setPrice(newPrice);
    return {};
}

// ========== DISPLAY LOGIC ==========

std::string ProductBus::toString(const ProductDto& p) {
    std::stringstream ss;
    ss << "ID: " << std::left << std::setw(6) << p.getID() << "| Name: " << std::left
       << std::setw(20) << p.getName() << "| Price: " << std::fixed << std::setprecision(2)
       << p.getPrice() << "| Stock: " << p.getStock();
    return ss.str();
}

void ProductBus::display(const ProductDto& p) {
    std::cout << toString(p) << std::endl;
}
