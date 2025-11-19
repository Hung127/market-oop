#ifndef CART_H_
#define CART_H_

#include <memory>
#include <vector>

#include "Product.h"

class Cart {
   private:
    std::vector<std::tuple<std::weak_ptr<Product>, int>> _items;
    double _totalPrice;
    std::expected<void, std::string> recalculateTotal();

   public:
    std::expected<void, std::string> add(const std::shared_ptr<Product>& p, int quantity = 1);
    void clear();
    std::expected<double, std::string> getTotal();

    /**
     * @brief Xóa sản phẩm hoàn toàn khỏi cart
     * @param productId ID sản phẩm cần xóa
     * @return true nếu xóa thành công, false nếu không tìm thấy
     */
    bool removeItem(const std::string& productId);

    /**
     * @brief Giảm số lượng sản phẩm trong cart
     *
     * - Nếu quantity sau khi giảm > 0: cập nhật quantity mới
     * - Nếu quantity sau khi giảm <= 0: xóa sản phẩm khỏi cart
     *
     * @param productId ID sản phẩm
     * @param quantity Số lượng cần giảm (mặc định = 1)
     * @return std::expected<void, std::string> - error nếu không tìm thấy hoặc
     * giảm quá nhiều
     */
    std::expected<void, std::string> reduceQuantity(const std::string& productId, int quantity = 1);

    /**
     * @brief Lấy số lượng của một sản phẩm trong cart
     * @param productId ID sản phẩm
     * @return Số lượng (0 nếu không có)
     */
    int getQuantity(const std::string& productId) const;

    /**
     * @brief Kiểm tra sản phẩm có trong cart không
     * @param productId ID sản phẩm
     * @return true nếu có
     */
    bool hasProduct(const std::string& productId) const;

    /**
     * @brief In nội dung cart ra màn hình
     */
    void displayCart() const;

   public:
    Cart();
    Cart(const std::vector<std::tuple<std::weak_ptr<Product>, int>>& items);
    std::vector<std::tuple<std::shared_ptr<Product>, int>> getItems() const;
};

#endif
