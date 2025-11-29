#include "../../include/BUS/Buyer_BUS.h"

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../../include/BUS/Buyer_BUS.h"
#include "../../include/BUS/Cart_BUS.h"
#include "../../include/BUS/Product_BUS.h"
#include "../../include/DAO/Buyer_DAO.h"
#include "../../include/DTO/Buyer_DTO.h"
#include "../../include/User.h"
#include "../../include/Utils/Utils.h"

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
void BuyerBus::addBalance(BuyerDto& buyer, double amount) {
    if (amount > 0) {
        // DTO chỉ chứa dữ liệu, ta dùng getter/setter
        buyer.setBalance(buyer.getBalance() + amount);
    }
}

// Thay thế cho Buyer::hasEnoughBalance
bool BuyerBus::hasEnoughBalance(const BuyerDto& buyer, double price) {
    return buyer.getBalance() >= price;
}

// ========== CART LOGIC ==========
// Các hàm này nhận vào đối tượng BuyerDto để thao tác trên Cart của nó

std::expected<void, std::string>
BuyerBus::addToCart(BuyerDto& BuyerDto, const std::shared_ptr<ProductDto>& p, int qty = 1) {
    return CartBus::add(BuyerDto.getCart(), p, qty);
}

bool BuyerBus::removeFromCart(BuyerDto& BuyerDto, const std::string& productId) {
    return CartBus::removeItem(BuyerDto.getCart(), productId);
}

std::expected<void, std::string>
BuyerBus::reduceCartQuantity(BuyerDto& BuyerDto, const std::string& productId, int qty) {
    return CartBus::reduceQuantity(BuyerDto.getCart(), productId, qty);
}

void BuyerBus::clearCart(BuyerDto& BuyerDto) {
    return CartBus::clear(BuyerDto.getCart());
}

// View Cart (Logic hiển thị, hoặc chỉ gọi getter của Model)
void BuyerBus::viewCart(const BuyerDto& BuyerDto) {
    return CartBus::displayCart(BuyerDto.getCart());
}

// ========== CHECKOUT LOGIC ==========
// Hàm logic phức tạp nhất, chuyển từ BuyerDto::checkout
std::expected<void, std::string> BuyerBus::checkout(BuyerDto& buyer) {
    CartDto& cart = buyer.getCart();

    // 1. Kiểm tra giỏ hàng rỗng (Nghiệp vụ)
    if (cart.getItems().empty()) {
        return std::unexpected("Your cart is EMPTY, cannot checkout !");
    }

    // 2. Get items before clearing cart
    const auto& items = cart.getItems();

    // 3. Reduce stock
    for (const auto& item : items) {
        const auto& [weakProduct, quantity] = item;

        // Phải lock() để lấy ra shared_ptr sử dụng được
        auto product = weakProduct.lock();

        if (!product) {
            // Nếu product == null nghĩa là sản phẩm đã bị xóa khỏi hệ thống
            return std::unexpected("There is an item in your cart that no longer exists!");
        }

        int currentStock = product->getStock();
        if (currentStock < quantity) {
            return std::unexpected(
                std::format("Product '{}' is out of stock (Remaining: {}, Buy: {})",
                            product->getName(), currentStock, quantity));
        }

        // Trừ tồn kho
        product->setStock(currentStock - quantity);
    }

    // 4. Tính tổng tiền
    auto totalPricePack = CartBus::getTotal(cart);
    if (!totalPricePack.has_value()) {
        return std::unexpected(totalPricePack.error());
    }
    double totalPrice = totalPricePack.value();

    // 5. Check balance
    if (!hasEnoughBalance(buyer, totalPrice)) {
        return std::unexpected(std::format("Insufficient balance. Need: ${:.2f}, Have: ${:.2f}",
                                           totalPrice, buyer.getBalance()));
    }

    // 5. Deduct balance
    buyer.setBalance(buyer.getBalance() - totalPrice);

    // 6. CREATE ORDER AND SAVE TO HISTORY
    std::vector<OrderItem> orderItems;
    for (const auto& [product, quantity] : items) {
        if (auto p = product.lock()) {
            orderItems.emplace_back(p, quantity);
        }
    }

    Order order(orderItems, totalPrice, Utils::getCurrentDate());
    buyer.getPurchasesHistory().addOrder(order);

    // 7. Clear cart
    CartBus::clear(cart);

    return {};
}

// ========== HISTORY LOGIC ==========
static void viewPurchaseHistory(const BuyerDto& BuyerDto);
