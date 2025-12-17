#ifndef SELLER_DTO_H_
#define SELLER_DTO_H_

#include <memory>
#include <string>
#include <vector>

#include "UserDTO.h"
class ProductDTO;

/**
 * @brief Simple data-transfer object for Seller (POJO).
 *
 * Responsibilities:
 *  - Chứa dữ liệu thuần: id, name, email (kế thừa User)
 *  - Quản lý danh sách product (add/remove/find) dưới dạng thao tác đơn giản
 *  - Không thực hiện validation phức tạp hay business logic (để Service xử lý)
 */
class SellerDTO : public UserDTO {
   private:
    // Seller owns products in inventory (choose shared_ptr or unique_ptr based on design)
    std::vector<std::shared_ptr<ProductDTO>> _myProducts;

   public:
    /**
     * @brief Tạo SellerDTO với đầy đủ thông tin
     * @param id Seller id
     * @param name Seller name
     * @param email Seller email
     */
    SellerDTO(const std::string& id, const std::string& name, const std::string& email,
              const std::string& password);

    ~SellerDTO() override = default;

    // SellerDTO(const SellerDTO&) = delete;
    // SellerDTO& operator=(const SellerDTO&) = delete;

    // // Allow move
    // SellerDTO(SellerDTO&&) noexcept = default;
    // SellerDTO& operator=(SellerDTO&&) noexcept = default;

    std::string getRole() const override;

    // Products accessors (read-only view)
    const std::vector<std::shared_ptr<ProductDTO>>& products() const;

    // Lightweight product operations (no heavy validation)
    void addProduct(std::shared_ptr<ProductDTO> product);
    bool removeProductById(const std::string& productId);  // returns true if removed
    std::shared_ptr<ProductDTO> findProductById(const std::string& productId) const;
};

#endif  // SELLER_DTO_H_
