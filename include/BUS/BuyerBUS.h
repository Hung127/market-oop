#ifndef BUS_BUYER_BUS_H_
#define BUS_BUYER_BUS_H_

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DAO/BuyerDAO.h"
#include "../DTO/BuyerDTO.h"
#include "../DTO/VoucherDTO.h"
#include "UserBUS.h"

class BuyerBUS : public UserBUS {
   public:
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
    static std::expected<std::pair<OrderDTO, std::vector<size_t>>, std::string> prepareOrderData(const BuyerDTO& buyer, const std::vector<std::string>& selectedProductIds, const std::vector<VoucherDTO*>& vouchers, bool useCoins);

    static std::expected<void, std::string> checkout(BuyerDTO& BuyerDTO, const std::vector<std::string>& selectedProductIds,vector<VoucherDTO*> voucher, bool useCoint);

    static std::expected<void, std::string> payment(BuyerDTO& buyer, const std::vector<std::string>& selectedProductIds, vector<VoucherDTO*> vouchers, bool useCoins) ;

    static std::expected<void, std::string> finalOrder(bool check, BuyerDTO& buyer, const std::vector<std::string>& selectedProductIds, vector<VoucherDTO*> vouchers, bool useCoins);

    // TODO: move this to ui
    // // ========== HISTORY LOGIC ==========
    // static void viewPurchaseHistory(const BuyerDTO& BuyerDTO);
};

#endif
