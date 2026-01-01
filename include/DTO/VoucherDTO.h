// include/DTO/VoucherDTO.h
#ifndef VOUCHER_DTO_H
#define VOUCHER_DTO_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <expected>
#include <format>
#include "CartDTO.h"
#include "OrderDTO.h"
#include "ProductDTO.h"

class VoucherDTO {
protected:
    std::string _id;
    std::string _code;
    std::string _ownerId; 
    std::string _expiryDate;

public:
    VoucherDTO(std::string id, std::string code, std::string owner, std::string expiryDate) 
        : _id(id), _code(code), _ownerId(owner), _expiryDate(expiryDate) {}
    virtual ~VoucherDTO() = default;

    std::string getCode() const { return _code; }
    std::string getOwnerId() const { return _ownerId; }
    std::string getExpiryDate() const { return _expiryDate; }
    // Hàm thuần ảo: Kiểm tra xem giỏ hàng có thỏa mãn điều kiện dùng mã không
    virtual std::expected<void, std::string> canApply(const OrderDTO& order) const = 0;
    // Hàm thuần ảo: Tính toán số tiền được giảm
    virtual double calculateDiscount(const OrderDTO& order) const = 0;
};

class SellerVoucherDTO: public VoucherDTO{
private:
    double _discountPercent;
    double _minOrderValue;
public:
    SellerVoucherDTO(std::string id, std::string code, std::string sellerId, double percent, double minVal, std::string expiryDate)
        : VoucherDTO(id, code, sellerId, expiryDate), _discountPercent(percent), _minOrderValue(minVal) {}
    virtual ~SellerVoucherDTO() = default;
    
    std::expected<void, std::string> canApply(const OrderDTO& order) const override {
        double totalShopValue = 0.0;

        if (order.date() > _expiryDate) {
            return std::unexpected(std::format("Voucher '{}' đã hết hạn!", _code));
        }

        for (const auto& item : order.items()) {
            // SỬA: Dùng -> thay vì .
            if (item->getSellerId() == _ownerId) {
                totalShopValue += item->getSubtotal(); 
            }
        }

        if (totalShopValue < _minOrderValue) {
            return std::unexpected(std::format("Voucher '{}' yêu cầu đơn tối thiểu {:.2f}.", _code, _minOrderValue));
        }
        return {};
    }

    double calculateDiscount(const OrderDTO& order) const override {
        double shopTotal = 0;
        for (const auto& item : order.items()) {
            // SỬA: Dùng -> thay vì .
            if (item->getSellerId() == _ownerId) 
                shopTotal += item->getPrice() * item->getQuantity(); 
        }
        return shopTotal * (_discountPercent / 100.0);
    }
};


#endif