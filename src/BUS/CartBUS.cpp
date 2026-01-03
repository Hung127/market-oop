#include "../../include/BUS/CartBUS.h"

#include <expected>
#include <iostream>
#include <memory>
#include <string>

#include "../../include/DTO/CartDTO.h"
#include "../../include/DTO/ProductDTO.h"

// Hàm nội bộ giúp tính lại tổng tiền và cập nhật vào DTO
std::expected<void, std::string> CartBUS::recalculateTotal(CartDTO& cart) {
    double total = 0.0;
    auto& items = cart.getItems();

    // Duyệt qua danh sách, loại bỏ các item đã bị hủy (hết hạn) nếu cần
    for (auto it = items.begin(); it != items.end();) {
        auto& [weakProduct, qty] = *it;

        // Convert weak_ptr sang shared_ptr để dùng
        if (auto product = weakProduct.lock()) {
            total += product->getPrice() * qty;  // Giả sử Product có hàm getPrice()
            ++it;
        } else {
            // Sản phẩm đã bị xóa khỏi bộ nhớ -> Xóa khỏi giỏ hàng luôn
            it = items.erase(it);
        }
    }
    cart.setTotalPrice(total);
    return {};
}
std::expected<void, std::string> CartBUS::add(CartDTO& cart, const std::shared_ptr<ProductDTO>& p,
                                              int quantity) {
    if (!p) {
        return std::unexpected("Product is invalid");
    }
    if (quantity <= 0) {
        return std::unexpected("Quantity must be positive");
    }

    auto& items = cart.getItems();
    bool found = false;

    // Kiểm tra xem sản phẩm đã có trong giỏ chưa
    for (auto& [weakProduct, qty] : items) {
        if (auto product = weakProduct.lock()) {
            if (product->getID() == p->getID()) {  // Giả sử Product có getId()
                qty += quantity;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        items.emplace_back(p, quantity);
    }

    return recalculateTotal(cart);
}

// Xóa sản phẩm
bool CartBUS::removeItem(CartDTO& cart, const std::string& productId) {
    auto& items = cart.getItems();
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (auto product = std::get<0>(*it).lock()) {
            if (product->getID() == productId) {
                items.erase(it);
                std::expected<void, std::string> result = recalculateTotal(cart);
                if (result.has_value()) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Giảm số lượng
std::expected<void, std::string>
CartBUS::reduceQuantity(CartDTO& cart, const std::string& productId, int quantity) {
    if (quantity <= 0) {
        return std::unexpected("Reduction quantity must be positive");
    }

    auto& items = cart.getItems();
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (auto product = std::get<0>(*it).lock()) {
            if (product->getID() == productId) {
                int& currentQty = std::get<1>(*it);
                currentQty -= quantity;

                if (currentQty <= 0) {
                    items.erase(it);  // Hết số lượng thì xóa luôn
                }

                std::expected<void, std::string> result = recalculateTotal(cart);
                if (!result.has_value()) {
                    return std::unexpected("Can not recalculateTotal");
                }

                return {};
            }
        }
    }
    return std::unexpected("Product not found in cart");
}

// Xóa sạch giỏ
void CartBUS::clear(CartDTO& cart) {
    cart.getItems().clear();
    cart.setTotalPrice(0.0);
}

// ========== CÁC HÀM TRA CỨU (READ-ONLY) ==========

int CartBUS::getQuantity(const CartDTO& cart, const std::string& productId) {
    for (const auto& [weakProduct, qty] : cart.getItems()) {
        if (auto product = weakProduct.lock()) {
            if (product->getID() == productId) {
                return qty;
            }
        }
    }
    return 0;
}

bool CartBUS::hasProduct(const CartDTO& cart, const std::string& productId) {
    return getQuantity(cart, productId) > 0;
}

// Hiển thị giỏ hàng
void CartBUS::displayCart(const CartDTO& cart) {
    std::cout << "--- Shopping Cart ---\n";
    if (cart.getItems().empty()) {
        std::cout << "(Empty)\n";
        return;
    }

    for (const auto& [weakProduct, qty] : cart.getItems()) {
        if (auto p = weakProduct.lock()) {
            std::cout << "- " << p->getName() << " | Qty: " << qty
                      << " | Price: " << p->getPrice() * qty << "\n";
        } else {
            std::cout << "- [Item Unavailable]\n";
        }
    }
    std::cout << "Total: " << cart.getTotalPrice() << "\n";
}

std::expected<double, std::string> CartBUS::getTotal(CartDTO& cart) {
    std::expected<void, std::string> result = recalculateTotal(cart);
    if (!result.has_value()) {
        return std::unexpected("Can not recalculate total");
    }

    return cart.getTotalPrice();
}
