#ifndef ORDERDTO_H
#define ORDERDTO_H

#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include "OrderItemDTO.h"

class OrderDTO {
   private:
    // --- GIỮ LẠI TỪ CODE CŨ (Để không lỗi DAO/Database) ---
    std::string _orderId;
    std::string _buyerId;
    std::vector<std::shared_ptr<OrderItemDTO>> _items; // Dùng shared_ptr như code cũ
    std::string _date;
    double _totalPrice;

    // --- BỔ SUNG TỪ CODE MỚI (Để phục vụ tính Voucher/Xu) ---
    double _rawTotal;        
    double _voucherDiscount; 
    double _coinDiscount;    

   public:
    OrderDTO();
    // Constructor gộp: Đảm bảo có ID và đầy đủ các trường tính toán
    OrderDTO(const std::string& orderId, const std::string& buyerId,
             const std::vector<std::shared_ptr<OrderItemDTO>>& items, double rawTotal,
             const std::string& date);

    // Accessors (Cả cũ và mới)
    const std::vector<std::shared_ptr<OrderItemDTO>>& items() const;
    double totalPrice() const;
    const std::string& date() const;
    const std::string& orderId() const;
    const std::string& buyerId() const;

    // Mutators
    void setItems(const std::vector<std::shared_ptr<OrderItemDTO>>& items);
    void setTotalPrice(double total);
    void setDate(const std::string& date);

    // --- CÁC HÀM MỚI ĐỂ XỬ LÝ VOUCHER ---
    void setDiscounts(double vDiscount, double cDiscount);
    double getRawTotal() const;
    double getVoucherDiscount() const;
    double getCoinDiscount() const;

    void addItem(const OrderItemDTO& item);
    std::shared_ptr<OrderItemDTO> findItemByProductId(const std::string& productId);
    void recalculateTotal();

    // Hàm hiển thị tính tiền mới của bạn
    void display() const {
        std::cout << "====== SỐ TIỀN CẦN THANH TOÁN ================================\n";
        std::cout << "Số tiền ban đầu:                    " << this->_rawTotal << "\n";
        std::cout << "Tiền giảm từ Voucher của shop:      " << this->_voucherDiscount << "\n";
        std::cout << "Tiền giảm từ xu:                    " << this->_coinDiscount << "\n";
        std::cout << "SỐ TIỀN CẦN THANH TOÁN:             " << this->_totalPrice << "\n";
        std::cout << "===============================================================\n";
    }
};

#endif