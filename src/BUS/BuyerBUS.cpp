#include "../../include/BUS/BuyerBUS.h"

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../../include/BUS/CartBUS.h"
#include "../../include/DTO/BuyerDTO.h"
#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"
#include "../../include/Utils/Utils.h"

// ========== BALANCE LOGIC ==========
void BuyerBUS::addBalance(BuyerDTO& buyer, double amount) {
    if (amount > 0) {
        // DTO chỉ chứa dữ liệu, ta dùng getter/setter
        buyer.setBalance(buyer.getBalance() + amount);
    }
}

// Thay thế cho Buyer::hasEnoughBalance
bool BuyerBUS::hasEnoughBalance(const BuyerDTO& buyer, double price) {
    return buyer.getBalance() >= price;
}

// ========== CART LOGIC ==========
// Các hàm này nhận vào đối tượng BuyerDTO để thao tác trên Cart của nó

std::expected<void, std::string>
BuyerBUS::addToCart(BuyerDTO& buyerDTO, const std::shared_ptr<ProductDTO>& p, int qty) {
    return CartBUS::add(buyerDTO.getCart(), p, qty);
}

bool BuyerBUS::removeFromCart(BuyerDTO& buyerDTO, const std::string& productId) {
    return CartBUS::removeItem(buyerDTO.getCart(), productId);
}

std::expected<void, std::string>
BuyerBUS::reduceCartQuantity(BuyerDTO& buyerDTO, const std::string& productId, int qty) {
    return CartBUS::reduceQuantity(buyerDTO.getCart(), productId, qty);
}

void BuyerBUS::clearCart(BuyerDTO& buyerDTO) {
    CartBUS::clear(buyerDTO.getCart());
}

// View Cart (Logic hiển thị, hoặc chỉ gọi getter của Model)
void BuyerBUS::viewCart(const BuyerDTO& buyerDTO) {
    CartBUS::displayCart(buyerDTO.getCart());
}

// ========== CHECKOUT LOGIC ==========
// Hàm logic phức tạp nhất, chuyển từ BuyerDTO::checkout
std::expected<void, std::string> BuyerBUS::checkout(BuyerDTO& buyer) {
    CartDTO& cart = buyer.getCart();

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
    auto totalPricePack = CartBUS::getTotal(cart);
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
    std::vector<OrderItemDTO> orderItems;
    for (const auto& [weakProduct, quantity] : items) {
        // Lấy ProductDTO từ Cart ra
        if (auto p = weakProduct.lock()) {
            // Trích xuất dữ liệu từ ProductDTO nạp vào OrderItem
            orderItems.emplace_back(p->getID(), p->getName(), p->getPrice(), quantity);
        }
    }

    OrderDTO order(orderItems, totalPrice, Utils::getCurrentDate());
    buyer.getPurchasesHistory().addOrder(order);

    // 7. Clear cart
    CartBUS::clear(cart);

    return {};
}
