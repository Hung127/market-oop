#ifndef BUS_PRODUCT_BUS_H
#define BUS_PRODUCT_BUS_H

#include <expected>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "../DTO/ProductDTO.h"
#include "../Utils/Utils.h"

class ProductDTO;

class ProductBUS {
   public:
    // ========== STOCK MANAGEMENT ==========
    static std::expected<void, std::string> reduceStock(ProductDTO& p, int quantity);
    static void addStock(ProductDTO& p, int quantity);
    static bool hasEnoughStock(const ProductDTO& p, int quantity);
    static bool isInStock(const ProductDTO& p);

    // ========== DATA UPDATE LOGIC (Validation) ==========
    static std::expected<void, std::string> updatePrice(ProductDTO& p, double newPrice);

    // ========== IMAGE HELPERS ==========
    // Validate & set an image path on product; persists change via DAO
    static std::expected<void, std::string> setImagePath(ProductDTO& p, const std::string& path);
    static std::expected<void, std::string> clearImagePath(ProductDTO& p);

    // ========== DISPLAY LOGIC ==========
    static std::string toString(const ProductDTO& p);
    static void display(const ProductDTO& p);
};

#endif  // BUS_PRODUCT_BUS_H
