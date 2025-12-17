#ifndef BUS_PRODUCT_BUS_H
#define BUS_PRODUCT_BUS_H

#include <expected>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

class ProductDTO;

class ProductBUS {
   public:
    // ========== STOCK MANAGEMENT ==========

    // Logic giảm tồn kho: Kiểm tra đủ hàng không, nếu đủ thì trừ
    static std::expected<void, std::string> reduceStock(ProductDTO& p, int quantity);

    // Logic thêm tồn kho
    static void addStock(ProductDTO& p, int quantity);

    // Logic kiểm tra tồn kho
    static bool hasEnoughStock(const ProductDTO& p, int quantity);

    static bool isInStock(const ProductDTO& p);

    // ========== DATA UPDATE LOGIC (Validation) ==========

    // Cập nhật giá an toàn (Logic cũ: if price > 0)
    static std::expected<void, std::string> updatePrice(ProductDTO& p, double newPrice);

    // ========== DISPLAY LOGIC ==========

    /**
     * @brief Logic tạo chuỗi hiển thị
     */
    static std::string toString(const ProductDTO& p);

    /**
     * @brief Logic in ra màn hình
     */
    static void display(const ProductDTO& p);
};

#endif  // BUS_PRODUCT_BUS_H
