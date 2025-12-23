#include "../../include/BUS/BuyerBUS.h"

#include <expected>
#include <memory>
#include <string>
#include <vector>
#include <format>

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
std::expected<void, std::string> BuyerBUS::checkout(BuyerDTO& buyer,const std::vector<std::string>& selectedProductIds, vector<VoucherDTO*> vouchers, bool useCoins) {
    CartDTO& cart = buyer.getCart();

    // 1. Kiểm tra giỏ hàng rỗng (Nghiệp vụ)
    if (cart.getItems().empty()) {
        return std::unexpected("Your cart is EMPTY, cannot checkout !");
    }

    // 2. Get items before clearing cart
    auto& cartItems = cart.getItems();
    std::vector<OrderItemDTO> orderItems;
    double rawTotal = 0.0;
    
    // Lưu lại vị trí các item trong giỏ hàng để xóa sau khi thanh toán xong
    std::vector<size_t> indicesToRemove;

    // 1. Duyệt giỏ hàng để lọc ra các món được chọn
    for (const std::string& selectedId : selectedProductIds) {
        // Tìm item trong giỏ hàng dựa trên ID
        auto it = std::find_if(cartItems.begin(), cartItems.end(), [&](const auto& item) {
            auto p = std::get<0>(item).lock();
            return p && p->getID() == selectedId;
        });

        // TRƯỜNG HỢP 1: Không tìm thấy ID này trong giỏ (Lỗi hệ thống/UI)
        if (it == cartItems.end()) {
            return std::unexpected(std::format("Product ID {} is not in your current session.", selectedId));
        }
        
        indicesToRemove.push_back(std::distance(cartItems.begin(), it));
        auto product = std::get<0>(*it).lock();
        int quantity = std::get<1>(*it);

        // TRƯỜNG HỢP 2: Sản phẩm đã bị xóa khỏi hệ thống (Dù vẫn còn trong giỏ)
        if (!product) {
            return std::unexpected("One of your selected items is no longer available in our store.");
        }

        // TRƯỜNG HỢP 3: Không đủ số lượng (Lỗi nghiệp vụ - Nhắc nhở người dùng)
        if (product->getStock() < quantity) {
            return std::unexpected(std::format(
                "Mặt hàng '{}' không đủ số lượng trong kho (Còn: {}, Bạn cần: {}).", 
                product->getName(), product->getStock(), quantity));
        }

        // Nếu mọi thứ ổn, thêm vào danh sách order
        orderItems.emplace_back(product->getID(), product->getSellerId(), 
                               product->getName(), product->getPrice(), quantity);
        rawTotal += product->getPrice() * quantity;
    }

    // 2. Tạo Order từ những món ĐÃ CHỌN
    OrderDTO order(orderItems, rawTotal, Utils::getCurrentDate());

    double totalDiscount=0.0;

    // 3.1 Áp dụng Voucher (nếu có)
    for (const auto& voucher : vouchers) {
        if (voucher && voucher->canApply(order)) {
            totalDiscount += voucher->calculateDiscount(order);
        }
    }
    order.setTotalPrice(rawTotal - totalDiscount);

    //3.2 Sử dụng coint nếu muốn
    double coinDiscount=0.0;
    if (useCoins) {
    coinDiscount = std::min(buyer.getCoins(), order.totalPrice());
    buyer.setCoins(buyer.getCoins() - coinDiscount); // Trừ xu ngay
    }

    double finalPrice = order.totalPrice() - coinDiscount;
    

    // 4. Kiểm tra số dư
    if (!hasEnoughBalance(buyer, finalPrice)) {
        return std::unexpected("Insufficient balance.");
    }

    // --- THỰC THI GIAO DỊCH ---
    
    // Trừ kho và Trừ tiền
    buyer.setBalance(buyer.getBalance() - finalPrice);
    for (auto idx : indicesToRemove) {
        if (auto p = std::get<0>(cartItems[idx]).lock()) {
            p->setStock(p->getStock() - std::get<1>(cartItems[idx]));
        }
    }

    // Thêm xu vào tài khoản
    double earnedCoins = finalPrice * 0.001;
    buyer.setCoins(buyer.getCoins() + earnedCoins);

    // Lưu vào lịch sử
    buyer.getPurchasesHistory().addOrder(order);

    // Xóa CHỈ những món đã thanh toán khỏi giỏ hàng
    // Xóa từ dưới lên để không làm lệch index
    std::sort(indicesToRemove.rbegin(), indicesToRemove.rend());
    for (auto idx : indicesToRemove) {
        cartItems.erase(cartItems.begin() + idx);
    }

    // Cập nhật lại tổng tiền giỏ hàng sau khi đã bớt đồ
    CartBUS::recalculateTotal(cart);

    return {};
}
