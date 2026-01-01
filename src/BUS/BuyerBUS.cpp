#include "../../include/BUS/BuyerBUS.h"

#include <expected>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <format>

#include "../../include/BUS/CartBUS.h"
#include "../../include/DAO/OrderDAO.h"
#include "../../include/DTO/BuyerDTO.h"
#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"
#include "../../include/DTO/SellerDTO.h"
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
// ========== CHECKOUT LOGIC ==========
std::expected<std::pair<OrderDTO, std::vector<size_t>>, std::string> 
BuyerBUS::prepareOrderData(const BuyerDTO& buyer, const std::vector<std::string>& selectedProductIds, 
                 const std::vector<VoucherDTO*>& vouchers, bool useCoins) {
    
    const CartDTO& cart = buyer.getCart();
    auto& cartItems = cart.getItems();

    if (cartItems.empty()) return std::unexpected("Your cart is EMPTY!");

    std::vector<OrderItemDTO> orderItems;
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

        if (!product) return std::unexpected("Item no longer available.");
        if (product->getStock() < quantity) 
            return std::unexpected(std::format("Mặt hàng '{}' không đủ kho.", product->getName()));

        orderItems.emplace_back(product->getID(), product->getSellerId(), 
                               product->getName(), product->getPrice(), quantity);
        rawTotal += product->getPrice() * quantity;
    }

    // --- LOGIC TÍNH TOÁN GIẢM GIÁ ---
    OrderDTO order(orderItems, rawTotal, Utils::getCurrentDate());
    double totalDiscount = 0.0;
    for (const auto& voucher : vouchers) {
        if (!voucher) continue;

        auto applyResult = voucher->canApply(order);
        
        if (applyResult.has_value()) {
            totalDiscount += voucher->calculateDiscount(order);
        } else {
            
            std::cout << "[THÔNG BÁO] " << applyResult.error() << std::endl;
        }
    }

    double priceAfterVoucher = rawTotal - totalDiscount;
    double coinDiscount = useCoins ? std::min(buyer.getCoins(), priceAfterVoucher) : 0.0;
    
    order.setDiscounts(totalDiscount, coinDiscount);
    order.setTotalPrice(priceAfterVoucher - coinDiscount);

    return std::make_pair(order, indicesToRemove);
}

// ========== CÁC HÀM PUBLIC ==========

std::expected<void, std::string> BuyerBUS::checkout(BuyerDTO& buyer, const std::vector<std::string>& ids, std::vector<VoucherDTO*> v, bool useC) {
    auto res = prepareOrderData(buyer, ids, v, useC);
    if (!res) return std::unexpected(res.error());

    res->first.display(); 
    return {}; 
}

std::expected<void, std::string> BuyerBUS::payment(BuyerDTO& buyer, const std::vector<std::string>& ids, std::vector<VoucherDTO*> v, bool useC) {
    auto res = prepareOrderData(buyer, ids, v, useC);
    if (!res) return std::unexpected(res.error());

    OrderDTO& order = res->first;
    std::vector<size_t>& indices = res->second;

    if (!hasEnoughBalance(buyer, order.totalPrice())) return std::unexpected("Tài khoản không đủ tiền để thanh toán. Vui lòng nạp thêm.");

    // THỰC THI GIAO DỊCH
    buyer.setBalance(buyer.getBalance() - order.totalPrice());
    buyer.setCoins(buyer.getCoins() - order.getCoinDiscount() + (order.totalPrice() * 0.001));

    auto& cartItems = buyer.getCart().getItems();
    for (auto idx : indices) {
        if (auto p = std::get<0>(cartItems[idx]).lock()) {
            p->setStock(p->getStock() - std::get<1>(cartItems[idx]));
        }
    }

    buyer.getPurchasesHistory().addOrder(order);
    std::sort(indices.rbegin(), indices.rend());
    for (auto idx : indices) cartItems.erase(cartItems.begin() + idx);
    
    CartBUS::recalculateTotal(buyer.getCart());
    return {}; 
}

std::expected<void, std::string> BuyerBUS::finalOrder(bool check, BuyerDTO& buyer, const std::vector<std::string>& ids, std::vector<VoucherDTO*> v, bool useC) {
    if (check) {
        return payment(buyer, ids, v, useC); 
    }
    return std::unexpected("Không thanh toán đơn hàng.");
}