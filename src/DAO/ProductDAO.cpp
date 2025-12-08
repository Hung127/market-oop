#include "../../include/DAO/Product_DAO.h"

#include <algorithm>
#include <iostream>
#include <vector>

static std::vector<std::shared_ptr<ProductDto>> mockProductDB;

std::expected<std::shared_ptr<ProductDto>, std::string>
ProductDao::getProductById(const std::string& id) {
    for (const auto& p : mockProductDB) {
        if (p->getID() == id) {
            return p;
        }
    }
    return std::unexpected("Product not found with ID: " + id);
}

// 2. Lấy tất cả sản phẩm (Ví dụ để hiển thị danh sách trang chủ)
std::vector<std::shared_ptr<ProductDto>> ProductDao::getAllProducts() {
    return mockProductDB;
}

// 3. Lấy sản phẩm theo Seller (Người bán)
std::vector<std::shared_ptr<ProductDto>>
ProductDao::getProductsBySeller(const std::string& sellerId) {
    std::vector<std::shared_ptr<ProductDto>> result;

    for (const auto& p : mockProductDB) {
        if (p->getSellerId() == sellerId) {
            result.push_back(p);
        }
    }
    return result;
}

// 4. Thêm sản phẩm mới (Insert)
bool ProductDao::insert(const ProductDto& product) {
    // Kiểm tra trùng ID
    for (const auto& p : mockProductDB) {
        if (p->getID() == product.getID()) {
            std::cerr << "[Error] Duplicate Product ID: " << product.getID() << "\n";
            return false;
        }
    }

    // Tạo bản sao của product, đóng gói vào shared_ptr và lưu vào vector
    mockProductDB.push_back(std::make_shared<ProductDto>(product));
    return true;
}

// 5. Cập nhật thông tin (Update)
bool ProductDao::update(const ProductDto& product) {
    for (auto& p : mockProductDB) {
        if (p->getID() == product.getID()) {
            // Ghi đè dữ liệu mới vào vùng nhớ cũ
            *p = product;
            return true;
        }
    }
    return false;  // Không tìm thấy ID để update
}

// 6. Xóa sản phẩm (Remove)
bool ProductDao::remove(const std::string& productId) {
    // Dùng thuật toán remove_if + erase idiom
    auto it = std::remove_if(
        mockProductDB.begin(), mockProductDB.end(),
        [&](const std::shared_ptr<ProductDto>& p) { return p->getID() == productId; });

    if (it != mockProductDB.end()) {
        mockProductDB.erase(it, mockProductDB.end());
        return true;
    }

    return false;  // Không tìm thấy
}