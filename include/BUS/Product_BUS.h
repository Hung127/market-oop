#ifndef BUS_PRODUCT_BUS_H
#define BUS_PRODUCT_BUS_H

#include <expected>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "../DTO/Product_DTO.h"
#include "../Seller.h"  // Cần để hiển thị thông tin Owner nếu cần

class ProductBus {
   public:
    // ========== STOCK MANAGEMENT ==========

    // Logic giảm tồn kho: Kiểm tra đủ hàng không, nếu đủ thì trừ
    static std::expected<void, std::string> reduceStock(ProductDto& p, int quantity);

    // Logic thêm tồn kho
    static void addStock(ProductDto& p, int quantity);

    // Logic kiểm tra tồn kho
    static bool hasEnoughStock(const ProductDto& p, int quantity);

    static bool isInStock(const ProductDto& p);

    // ========== DATA UPDATE LOGIC (Validation) ==========

    // Cập nhật giá an toàn (Logic cũ: if price > 0)
    static std::expected<void, std::string> updatePrice(ProductDto& p, double newPrice);

    // ========== DISPLAY LOGIC ==========

    /**
     * @brief Logic tạo chuỗi hiển thị
     */
    static std::string toString(const ProductDto& p);

    /**
     * @brief Logic in ra màn hình
     */
    static void display(const ProductDto& p);
};

#endif  // BUS_PRODUCT_BUS_H