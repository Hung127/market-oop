#ifndef SELLER_BUS_H_
#define SELLER_BUS_H_

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DTO/ProductDTO.h"

class SellerDTO;

/**
 * Business error codes returned by SellerBUS.
 */
enum class BusError { ValidationFailed, NotFound, Conflict, MarketError, InternalError };

class SellerBUS {
   private:
    std::shared_ptr<SellerDTO> _seller;

    // Helper
    std::shared_ptr<ProductDTO> findProductInInventory(const std::string& id) const;

   private:
    SellerBUS(const std::shared_ptr<SellerDTO>& seller) : _seller(seller) {
        // Do nothing
    }

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
     * @return expected chứa shared_ptr tới product vừa tạo, hoặc BusError on failure
     */
    std::expected<std::shared_ptr<ProductDTO>, BusError>
    createProduct(const std::string& id, const std::string& name, double price, int stock);

    /**
     * @brief Xóa sản phẩm khỏi inventory
     *
     * Sẽ tự động unlist khỏi market nếu product đã được publish.
     *
     * @param market Market instance
     * @param productId ID sản phẩm cần xóa
     * @return expected<void, BusError>
     */
    // std::expected<void, BusError> deleteProduct(Market& market, const std::string& productId);

    /**
     * @brief Cập nhật thông tin sản phẩm
     *
     * @param productId ID sản phẩm
     * @param newName Tên mới (để trống nếu không đổi)
     * @param newPrice Giá mới (-1 nếu không đổi)
     * @return expected<void, BusError>
     */
    std::expected<void, BusError> updateProduct(const std::string& productId,
                                                const std::string& newName = "",
                                                double newPrice = -1);

    /**
     * @brief Cập nhật stock
     */
    std::expected<void, BusError> updateStock(const std::string& productId, int newStock);

    /**
     * @brief Lấy tất cả products trong inventory (bao gồm cả chưa publish)
     */
    std::expected<std::vector<std::shared_ptr<ProductDTO>>, BusError> getMyProducts() const;

    /**
     * @brief Lấy số lượng products trong inventory
     */
    std::expected<int, BusError> getProductCount() const;

    // ========== MARKET INTERACTION ==========

    /**
     * @brief Publish sản phẩm lên market
     *
     * Sau khi publish, buyer có thể tìm thấy product này qua search.
     *
     * @param market Market instance
     * @param productId ID sản phẩm cần publish
     * @return expected<void, BusError>
     */
    // std::expected<void, BusError> publishToMarket(Market& market, const std::string& productId);

    /**
     * @brief Unpublish sản phẩm khỏi market
     *
     * Product vẫn nằm trong inventory nhưng buyer không thấy được.
     * Hữu ích khi muốn tạm ngưng bán mà không xóa product.
     *
     * @param market Market instance
     * @param productId ID sản phẩm cần unpublish
     * @return expected<void, BusError>
     */
    // std::expected<void, BusError> unpublishFromMarket(Market& market, const std::string&
    // productId);

    /**
     * @brief Tìm sản phẩm trong inventory theo tên
     */
    std::vector<std::shared_ptr<ProductDTO>>  // NOLINT
    searchMyProductsByName(const std::string& keyword) const;

    /**
     * @brief Tìm sản phẩm gần đúng nhất trong inventory (fuzzy search)
     */
    std::shared_ptr<ProductDTO> searchMyClosestProduct(const std::string& keyword) const;  // NOLINT

    static std::expected<SellerBUS, BusError> create(std::shared_ptr<SellerDTO> seller);
};

#endif
