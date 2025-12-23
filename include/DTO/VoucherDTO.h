// include/DTO/VoucherDTO.h
#ifndef VOUCHER_DTO_H
#define VOUCHER_DTO_H

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include<expected>
#include "CartDTO.h"
#include "OrderDTO.h"
#include "ProductDTO.h"

class VoucherDTO {
protected:
    std::string _id;
    std::string _code;
    std::string _ownerId; 

public:
    VoucherDTO(std::string id, std::string code, std::string owner) 
        : _id(id), _code(code), _ownerId(owner) {}
    virtual ~VoucherDTO() = default;

    std::string getCode() const { return _code; }
    std::string getOwnerId() const { return _ownerId; }

    // Hàm thuần ảo: Kiểm tra xem giỏ hàng có thỏa mãn điều kiện dùng mã không
    virtual bool canApply(const OrderDTO& order) const = 0;

    // Hàm thuần ảo: Tính toán số tiền được giảm
    virtual double calculateDiscount(const OrderDTO& order) const = 0;
};

class SellerVoucherDTO: public VoucherDTO{
private:
    double _discountPercent;
    double _minOrderValue;
public:
    SellerVoucherDTO(std::string id, std::string code, std::string sellerId, double percent, double minVal)
        : VoucherDTO(id, code, sellerId), _discountPercent(percent), _minOrderValue(minVal) {}
    
    bool canApply(const OrderDTO& order) const override {
        double totalShopValue = 0.0;

        for (const auto& item : order.items()) {
            if (item.getSellerId() == _ownerId) {
                totalShopValue += item.getSubtotal(); // Cộng dồn giá trị từng món
            }
        }

        // Voucher chỉ có hiệu lực nếu tổng tiền của shop đạt mức tối thiểu
        return totalShopValue >= _minOrderValue;
    }

    //
    double calculateDiscount(const OrderDTO& order) const override {
        double shopTotal = 0;
        for (const auto& item : order.items()) {
            if (item.getSellerId() == _ownerId) 
                shopTotal += item.getPrice() * item.getQuantity(); // Dùng snapshot giá trong Order
        }
        return shopTotal * (_discountPercent / 100.0);
    }

};


#endif