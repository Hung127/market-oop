#ifndef BUS_BuyerDto_BUS_H
    #define BUS_BUYER_BUS_H

    #include <expected>
    #include <memory>
    #include <string>
    #include <vector>

    #include "../DAO/Buyer_DAO.h"
    #include "../DTO/Buyer_DTO.h"

class BuyerBus : public User {
   public:
    // Logic tạo Buyer (Validation -> gọi DAO save -> trả về Buyer)
    // Thay thế cho Buyer::create cũ
    static std::expected<std::unique_ptr<BuyerDto>, std::string>
    create(const std::string& id, const std::string& name, const std::string& email,
           const std::string& password, double balance = 0.0);

    // ========== BALANCE LOGIC ==========
    // Thay thế cho Buyer::addBalance

    // Thay thế cho Buyer::hasEnoughBalance
    static bool hasEnoughBalance(const BuyerDto& BuyerDto, double price);

    // ========== CART LOGIC ==========
    // Các hàm này nhận vào đối tượng BuyerDto để thao tác trên Cart của nó

    static std::expected<void, std::string>
    addToCart(BuyerDto& BuyerDto, const std::shared_ptr<Product>& p, int qty = 1);

    static bool removeFromCart(BuyerDto& BuyerDto, const std::string& productId);

    static std::expected<void, std::string>
    reduceCartQuantity(BuyerDto& BuyerDto, const std::string& productId, int qty = 1);

    static void clearCart(BuyerDto& BuyerDto);

    // View Cart (Logic hiển thị, hoặc chỉ gọi getter của Model)
    static void viewCart(const BuyerDto& BuyerDto);

    // ========== CHECKOUT LOGIC ==========
    // Hàm logic phức tạp nhất, chuyển từ BuyerDto::checkout
    static std::expected<void, std::string> checkout(BuyerDto& BuyerDto);

    // ========== HISTORY LOGIC ==========
    static void viewPurchaseHistory(const BuyerDto& BuyerDto);
};

#endif