#ifndef CART_BUS_H
    #define BUS_BUS_H

    #include <expected>
    #include <iostream>
    #include <memory>
    #include <string>

    #include "../DTO/CartDTO.h"
    #include "../DTO/ProductDTO.h"

class CartBUS {
   public:
    // ========== HÀM HỖ TRỢ TÍNH TOÁN (UTILITIES) ==========

    /**
     * @brief Tính lại tổng tiền (Tên hàm giữ nguyên như code gốc)
     * Đã chuyển từ Private sang Public để có thể gọi từ bên ngoài.
     */
    static std::expected<void, std::string> recalculateTotal(CartDTO& cart);

    // ========== CÁC HÀM LOGIC CHÍNH (BUSINESS LOGIC) ==========

    // Thêm sản phẩm
    static std::expected<void, std::string> add(CartDTO& cart, const std::shared_ptr<ProductDTO>& p,
                                                int quantity = 1);

    // Xóa sản phẩm
    static bool removeItem(CartDTO& cart, const std::string& productId);

    // Giảm số lượng
    static std::expected<void, std::string>
    reduceQuantity(CartDTO& cart, const std::string& productId, int quantity = 1);

    // Xóa sạch giỏ
    static void clear(CartDTO& cart);

    // ========== CÁC HÀM TRA CỨU (READ-ONLY) ==========

    // Lấy tổng tiền (Hàm này ánh xạ từ Cart::getTotal cũ)
    static std::expected<double, std::string> getTotal(CartDTO& cart);

    static int getQuantity(const CartDTO& cart, const std::string& productId);

    static bool hasProduct(const CartDTO& cart, const std::string& productId);

    // Hiển thị
    static void displayCart(const CartDTO& cart);
};

#endif  // BUS_CART_SERVICE_H
