#ifndef DAO_PRODUCT_DAO_H
#define DAO_PRODUCT_DAO_H

#include <expected>
#include <fstream>
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

    // Lưu ảnh
    void saveToFile(const std::string& fileName, const ProductExtraInfoDTO& dto) {
        std::ofstream f(fileName, std::ios::binary);
        if (!f)
            return;

        // 1. Ghi mô tả
        size_t descLen = dto.getDescription().size();
        f.write(reinterpret_cast<char*>(&descLen), sizeof(descLen));
        f.write(dto.getDescription().c_str(), descLen);

        // 2. Ghi số lượng ảnh
        size_t count = dto.getImageCount();
        f.write(reinterpret_cast<char*>(&count), sizeof(count));

        // 3. Ghi từng tấm ảnh
        for (const auto& img : dto.getImageRawData()) {
            size_t imgSize = img.size();
            f.write(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
            f.write(reinterpret_cast<const char*>(img.data()), imgSize);
        }
        f.close();
    }

    void loadFromFile(const std::string& fileName, ProductExtraInfoDTO& dto) {
        std::ifstream f(fileName, std::ios::binary);
        if (!f)
            return;

        // 1. Đọc lại mô tả
        size_t descLen;
        f.read(reinterpret_cast<char*>(&descLen), sizeof(descLen));
        std::string desc(descLen, ' ');
        f.read(&desc[0], descLen);
        dto.setDescription(desc);

        // 2. Đọc lại số lượng ảnh
        size_t count;
        f.read(reinterpret_cast<char*>(&count), sizeof(count));

        // 3. Đọc lại từng tấm ảnh
        for (size_t i = 0; i < count; ++i) {
            size_t imgSize;
            f.read(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));

            std::vector<uint8_t> imgData(imgSize);
            f.read(reinterpret_cast<char*>(imgData.data()), imgSize);
            dto.addImageData(imgData);  // Nạp lại vào DTO để Qt hiển thị
        }
        f.close();
    }
};

#endif  // DAO_PRODUCT_DAO_H
