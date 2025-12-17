#ifndef DAO_PRODUCT_DAO_H
#define DAO_PRODUCT_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DTO/ProductDTO.h"

class ProductDAO {
   public:
    // Tìm sản phẩm theo ID
    static std::expected<std::shared_ptr<ProductDTO>, std::string>
    getProductById(const std::string& id);

    // Lấy tất cả sản phẩm
    static std::vector<std::shared_ptr<ProductDTO>> getAllProducts();

    // Lấy tất cả sản phẩm của một Seller cụ thể
    static std::vector<std::shared_ptr<ProductDTO>>
    getProductsBySeller(const std::string& sellerId);

    // Lưu sản phẩm mới vào DB
    static bool insert(const ProductDTO& product);

    static bool insert(const std::shared_ptr<ProductDTO>& product);
    // Cập nhật thông tin sản phẩm (ví dụ sau khi giảm stock)
    static bool update(const ProductDTO& product);

    // Xóa sản phẩm
    static bool remove(const std::string& productId);
};

#endif  // DAO_PRODUCT_DAO_H
