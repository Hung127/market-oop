#ifndef CART_BUS_H
    #define BUS_BUS_H

    #include <expected>
    #include <iostream>
    #include <memory>
    #include <string>

    #include "../DTO/Cart_DTO.h"
    #include "../DTO/Product_DTO.h"

class CartBus {
   public:
    // ========== HÀM HỖ TRỢ TÍNH TOÁN (UTILITIES) ==========

    /**
     * @brief Tính lại tổng tiền (Tên hàm giữ nguyên như code gốc)
     * Đã chuyển từ Private sang Public để có thể gọi từ bên ngoài.
     */
    static std::expected<void, std::string> recalculateTotal(CartDto& cart);

    // ========== CÁC HÀM LOGIC CHÍNH (BUSINESS LOGIC) ==========

    // Thêm sản phẩm
    static std::expected<void, std::string> add(CartDto& cart, const std::shared_ptr<ProductDto>& p,
                                                int quantity = 1);

    // Xóa sản phẩm
    static bool removeItem(CartDto& cart, const std::string& productId);

    // Giảm số lượng
    static std::expected<void, std::string>
    reduceQuantity(CartDto& cart, const std::string& productId, int quantity = 1);

    // Xóa sạch giỏ
    static void clear(CartDto& cart);

    // ========== CÁC HÀM TRA CỨU (READ-ONLY) ==========

    // Lấy tổng tiền (Hàm này ánh xạ từ Cart::getTotal cũ)
    static std::expected<double, std::string> getTotal(CartDto& cart);

    static int getQuantity(const CartDto& cart, const std::string& productId);

    static bool hasProduct(const CartDto& cart, const std::string& productId);

    // Hiển thị
    static void displayCart(const CartDto& cart);
};

#endif  // BUS_CART_SERVICE_H