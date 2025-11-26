#include "../../include/BUS/Buyer_BUS.h"

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../../include/DAO/Buyer_DAO.h"
#include "../../include/DTO/Buyer_DTO.h"
#include "../../include/User.h"

std::expected<std::unique_ptr<BuyerDto>, std::string>
BuyerBus::create(const std::string& id, const std::string& name, const std::string& email,
                 const std::string& password, double balance) {
    // check and add id
    for (const std::string& usedId : User::_ids) {
        if (usedId == id) {
            return std::unexpected("User id is already used");
        }
    }

    // add to used id
    User::_ids.insert(id);

    return std::unique_ptr<BuyerDto>(new BuyerDto(id, name, email, password, balance));
}

// ========== BALANCE LOGIC ==========
// Thay thế cho Buyer::addBalance
static void addBalance(BuyerDto& buyer, double amount);

// Thay thế cho Buyer::hasEnoughBalance
bool hasEnoughBalance(const BuyerDto& BuyerDto, double price) {
    return (this->_balance >= price);
}

// ========== CART LOGIC ==========
// Các hàm này nhận vào đối tượng BuyerDto để thao tác trên Cart của nó

static std::expected<void, std::string> addToCart(BuyerDto& BuyerDto,
                                                  const std::shared_ptr<Product>& p, int qty = 1);

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
