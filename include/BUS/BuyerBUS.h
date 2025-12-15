#ifndef BUS_BUYER_BUS_H_
#define BUS_BUYER_BUS_H_

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DAO/BuyerDAO.h"
#include "../DTO/BuyerDTO.h"
#include "UserBUS.h"

class BuyerBUS : public UserBUS {
   public:
    // Logic tạo Buyer (Validation -> gọi DAO save -> trả về Buyer)
    // Thay thế cho Buyer::create cũ
    static std::expected<std::unique_ptr<BuyerDTO>, std::string>
    create(const std::string& id, const std::string& name, const std::string& email,
           const std::string& password, double balance = 0.0);

    // ========== BALANCE LOGIC ==========
    // Thay thế cho Buyer::addBalance
    static void addBalance(BuyerDTO& buyer, double amount);

    // Thay thế cho Buyer::hasEnoughBalance
    static bool hasEnoughBalance(const BuyerDTO& BuyerDTO, double price);

    // ========== CART LOGIC ==========
    // Các hàm này nhận vào đối tượng BuyerDTO để thao tác trên Cart của nó

    static std::expected<void, std::string>
    addToCart(BuyerDTO& BuyerDTO, const std::shared_ptr<ProductDTO>& p, int qty = 1);

    static bool removeFromCart(BuyerDTO& BuyerDTO, const std::string& productId);

    static std::expected<void, std::string>
    reduceCartQuantity(BuyerDTO& BuyerDTO, const std::string& productId, int qty = 1);

    static void clearCart(BuyerDTO& BuyerDTO);

    // View Cart (Logic hiển thị, hoặc chỉ gọi getter của Model)
    static void viewCart(const BuyerDTO& BuyerDTO);

    // ========== CHECKOUT LOGIC ==========
    // Hàm logic phức tạp nhất, chuyển từ BuyerDTO::checkout
    static std::expected<void, std::string> checkout(BuyerDTO& BuyerDTO);

    // ========== HISTORY LOGIC ==========
    static void viewPurchaseHistory(const BuyerDTO& BuyerDTO);
};

#endif
