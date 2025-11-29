#ifndef DAO_PRODUCT_DAO_H
#define DAO_PRODUCT_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DTO/Product_DTO.h"

class ProductDao {
   public:
    // Tìm sản phẩm theo ID
    static std::expected<std::shared_ptr<ProductDto>, std::string>
    getProductById(const std::string& id);

    // Lấy tất cả sản phẩm
    static std::vector<std::shared_ptr<ProductDto>> getAllProducts();

    // Lấy tất cả sản phẩm của một Seller cụ thể
    static std::vector<std::shared_ptr<ProductDto>>
    getProductsBySeller(const std::string& sellerId);

    // Lưu sản phẩm mới vào DB
    static bool insert(const ProductDto& product);

    // Cập nhật thông tin sản phẩm (ví dụ sau khi giảm stock)
    static bool update(const ProductDto& product);

    // Xóa sản phẩm
    static bool remove(const std::string& productId);
};

#endif  // DAO_PRODUCT_DAO_H