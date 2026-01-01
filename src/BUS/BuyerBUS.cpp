#include "../../include/BUS/BuyerBUS.h"

#include <expected>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <format>
#include <algorithm>

#include "../../include/BUS/CartBUS.h"
#include "../../include/DAO/OrderDAO.h"
#include "../../include/DTO/BuyerDTO.h"
#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"
#include "../../include/DTO/SellerDTO.h"
#include "../../include/DTO/VoucherDTO.h"
#include "../../include/Utils/Utils.h"

// ========== BALANCE LOGIC (Giữ nguyên) ==========
void BuyerBUS::addBalance(BuyerDTO& buyer, double amount) {
    if (amount > 0) {
        buyer.setBalance(buyer.getBalance() + amount);
    }
}

bool BuyerBUS::hasEnoughBalance(const BuyerDTO& buyer, double price) {
    return buyer.getBalance() >= price;
}

// ========== CART LOGIC (Giữ nguyên) ==========
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

void BuyerBUS::viewCart(const BuyerDTO& buyerDTO) {
    CartBUS::displayCart(buyerDTO.getCart());
}

// ========== CHECKOUT LOGIC (Giải quyết xung đột) ==========
std::expected<std::pair<OrderDTO, std::vector<size_t>>, std::string> 
BuyerBUS::prepareOrderData(const BuyerDTO& buyer, const std::vector<std::string>& selectedProductIds, 
                 const std::vector<VoucherDTO*>& vouchers, bool useCoins) {
    
    const CartDTO& cart = buyer.getCart();
    auto& cartItems = cart.getItems();

    // 1. Kiểm tra giỏ hàng rỗng (Logic từ main)
    if (cartItems.empty()) return std::unexpected("Your cart is EMPTY!");

    // 2. Chuẩn bị danh sách item sử dụng shared_ptr để khớp với Header
    std::vector<std::shared_ptr<OrderItemDTO>> orderItems;
    double rawTotal = 0.0;
    std::vector<size_t> indicesToRemove;

    // --- LOGIC KIỂM TRA HÀNG HÓA ---
    for (const std::string& selectedId : selectedProductIds) {
        auto it = std::find_if(cartItems.begin(), cartItems.end(), [&](const auto& item) {
            auto p = std::get<0>(item).lock();
            return p && p->getID() == selectedId;
        });

        if (it == cartItems.end()) 
            return std::unexpected(std::format("Product ID {} not in cart.", selectedId));
        
        indicesToRemove.push_back(std::distance(cartItems.begin(), it));
        auto product = std::get<0>(*it).lock();
        int quantity = std::get<1>(*it);

        // 3. Kiểm tra tồn kho (Logic từ main)
        if (!product) return std::unexpected("Item no longer available.");
        if (product->getStock() < quantity) 
            return std::unexpected(std::format("Mặt hàng '{}' không đủ kho.", product->getName()));

        // 4. Tạo OrderItemDTO với đầy đủ thông tin (bao gồm SellerName)
        std::string sellerName = product->getOwner() ? product->getOwner()->getName() : "Unknown";
        auto newItem = std::make_shared<OrderItemDTO>(
            product->getID(), product->getName(), product->getSellerId(), 
            sellerName, product->getPrice(), quantity
        );
        
        orderItems.push_back(newItem);
        rawTotal += product->getPrice() * quantity;
    }

    // --- LOGIC TÍNH TOÁN GIẢM GIÁ (Giữ logic từ HEAD) ---
    // Khởi tạo với 5 tham số theo OrderDTO.h mới
    OrderDTO order(Utils::generateId(), buyer.getId(), orderItems, rawTotal, Utils::getCurrentDate());
    
    double totalDiscount = 0.0;
    for (const auto& voucher : vouchers) {
        if (!voucher) continue;

        // Sử dụng hàm canApply từ VoucherDTO
        auto applyResult = voucher->canApply(order);
        if (applyResult.has_value()) {
            totalDiscount += voucher->calculateDiscount(order);
        } else {
            std::cout << "[THÔNG BÁO] " << applyResult.error() << std::endl;
        }
    }

    double priceAfterVoucher = rawTotal - totalDiscount;
    double coinDiscount = useCoins ? std::min(buyer.getCoins(), priceAfterVoucher) : 0.0;
    
    // Lưu kết quả tính toán vào OrderDTO
    order.setDiscounts(totalDiscount, coinDiscount);
    order.setTotalPrice(priceAfterVoucher - coinDiscount);

    return std::make_pair(order, indicesToRemove);
}

// ========== CÁC HÀM THỰC THI  ==========

std::expected<void, std::string> BuyerBUS::checkout(BuyerDTO& buyer, const std::vector<std::string>& ids, std::vector<VoucherDTO*> v, bool useC) {
    auto res = prepareOrderData(buyer, ids, v, useC);
    if (!res) return std::unexpected(res.error());

    res->first.display(); 
    return {}; 
}

std::expected<void, std::string> BuyerBUS::payment(BuyerDTO& buyer, const std::vector<std::string>& ids, vector<VoucherDTO*> v, bool useC) {
    auto res = prepareOrderData(buyer, ids, v, useC);
    if (!res) return std::unexpected(res.error());

    // 'order' là tham chiếu đến object nằm trong 'res'
    OrderDTO& order = res->first; 
    std::vector<size_t>& indices = res->second;

    // 1. Kiểm tra số dư (Sử dụng tham chiếu 'buyer' trực tiếp)
    if (!hasEnoughBalance(buyer, order.totalPrice())) 
        return std::unexpected("Tài khoản không đủ tiền.");

    // 2. TRỪ TIỀN (Đây là dòng sẽ sửa lỗi Balance = 100)
    double amountToPay = order.totalPrice();
    buyer.setBalance(buyer.getBalance() - amountToPay);

    // 3. Cập nhật Xu (Coins)
    buyer.setCoins(buyer.getCoins() - order.getCoinDiscount() + (amountToPay * 0.001));

    // 4. Trừ kho sản phẩm
    auto& cartItems = buyer.getCart().getItems();
    for (auto idx : indices) {
        if (auto p = std::get<0>(cartItems[idx]).lock()) {
            p->setStock(p->getStock() - std::get<1>(cartItems[idx]));
        }
    }

    // 5. LƯU VÀO LỊCH SỬ VÀ DAO
    buyer.getPurchasesHistory().addOrder(order);
    
    // Xóa khỏi giỏ hàng
    std::sort(indices.rbegin(), indices.rend());
    for (auto idx : indices) {
        cartItems.erase(cartItems.begin() + idx);
    }
    CartBUS::recalculateTotal(buyer.getCart());

    // QUAN TRỌNG: Lưu vào DAO để Unit Test có thể lấy ra
    OrderDAO::addOrder(order); 
    
    return {}; 
}

std::expected<void, std::string> BuyerBUS::finalOrder(bool check, BuyerDTO& buyer, const std::vector<std::string>& ids, std::vector<VoucherDTO*> v, bool useC) {
    if (check) {
        return payment(buyer, ids, v, useC); 
    }
    return std::unexpected("Không thanh toán đơn hàng.");
}