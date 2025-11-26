#ifndef SELLER_BUS_H_
#define SELLER_BUS_H_

#include <expected>
#include <memory>
#include <vector>

#include "../User.h"
#include "../dto/SellerDTO.h"

class Market;
class Product;

class SellerBUS {
   private:
    std::shared_ptr<SellerDTO> _seller;

    // Helper
    std::shared_ptr<Product> findProductInInventory(const std::string& id);

   public:
    // ========== PRODUCT MANAGEMENT ==========

    /**
     * @brief Tạo sản phẩm mới trong inventory
     *
     * Product được lưu trong inventory của seller, chưa được publish lên
     * market. Seller có thể tạo draft products trước khi quyết định publish.
     *
     * @param id ID sản phẩm (phải unique trong inventory)
     * @param name Tên sản phẩm
     * @param price Giá sản phẩm (phải > 0)
     * @param stock Số lượng tồn kho
     * @return shared_ptr tới product vừa tạo, nullptr nếu ID trùng
     */
    std::shared_ptr<Product> createProduct(const std::string& id, const std::string& name,
                                           double price, int stock);

    /**
     * @brief Xóa sản phẩm khỏi inventory
     *
     * Sẽ tự động unlist khỏi market nếu product đã được publish.
     *
     * @param market Market instance
     * @param productId ID sản phẩm cần xóa
     * @return true nếu xóa thành công
     */
    bool deleteProduct(Market& market, const std::string& productId);

    /**
     * @brief Cập nhật thông tin sản phẩm
     *
     * @param productId ID sản phẩm
     * @param newName Tên mới (để trống nếu không đổi)
     * @param newPrice Giá mới (-1 nếu không đổi)
     * @return true nếu cập nhật thành công
     */
    bool updateProduct(const std::string& productId, const std::string& newName = "",
                       double newPrice = -1);

    /**
     * @brief Cập nhật stock
     */
    bool updateStock(const std::string& productId, int newStock);

    /**
     * @brief Lấy tất cả products trong inventory (bao gồm cả chưa publish)
     */
    std::vector<std::shared_ptr<Product>> getMyProducts() const;

    /**
     * @brief Lấy số lượng products trong inventory
     */
    int getProductCount() const {  // TODO: Make it work
        return 0;
    }

    // ========== MARKET INTERACTION ==========

    /**
     * @brief Publish sản phẩm lên market
     *
     * Sau khi publish, buyer có thể tìm thấy product này qua search.
     *
     * @param market Market instance
     * @param productId ID sản phẩm cần publish
     * @return true nếu publish thành công
     */
    bool publishToMarket(Market& market, const std::string& productId);

    /**
     * @brief Unpublish sản phẩm khỏi market
     *
     * Product vẫn nằm trong inventory nhưng buyer không thấy được.
     * Hữu ích khi muốn tạm ngưng bán mà không xóa product.
     *
     * @param market Market instance
     * @param productId ID sản phẩm cần unpublish
     * @return true nếu unpublish thành công
     */
    bool unpublishFromMarket(Market& market, const std::string& productId);

    // ========== SEARCH IN INVENTORY ==========

    /**
     * @brief Tìm sản phẩm trong inventory theo tên
     */
    std::vector<std::shared_ptr<Product>> searchMyProductsByName(const std::string& keyword) const;

    /**
     * @brief Tìm sản phẩm gần đúng nhất trong inventory (fuzzy search)
     */
    std::shared_ptr<Product> searchMyClosestProduct(const std::string& keyword) const;
};

#endif
