#ifndef ORDERDTO_H
#define ORDERDTO_H

#include <string>
#include <iostream>
#include <vector>
#include "OrderItemDTO.h"

class OrderDTO {
   private:
    std::vector<OrderItemDTO> _items;
    std::string _date;
    
    // Tiền cuối cùng (Biến cũ - Giữ nguyên)
    double _totalPrice;

    // Các trường mới để phục vụ "Hiện cách tính tiền"
    double _rawTotal;        // Tổng tiền gốc
    double _voucherDiscount; // Tiền giảm từ Voucher
    double _coinDiscount;    // Tiền giảm từ Xu

   public:
    // --- CÁC HÀM CŨ (GIỮ NGUYÊN 100%) ---
    OrderDTO();
    OrderDTO(const std::vector<OrderItemDTO>& items, double rawTotal, const std::string& date);

    const std::vector<OrderItemDTO>& items() const;
    double totalPrice() const; // Hàm cũ
    const std::string& date() const;

    void setItems(const std::vector<OrderItemDTO>& items);
    void setTotalPrice(double total); // Hàm cũ
    void setDate(const std::string& date);

    void addItem(const OrderItemDTO& item);
    void recalculateTotal(); 

    // --- CÁC HÀM MỚI BỔ SUNG ---
    void setDiscounts(double vDiscount, double cDiscount);
    double getRawTotal() const;
    double getVoucherDiscount() const;
    double getCoinDiscount() const;

    //---In ra cách tính tiền---
    void display(){
        std::cout <<"====== SỐ TIỀN CẦN THANH TOÁN================================\n";
        std::cout<<"Số tiền ban đầu:                    "<<this->_rawTotal<<"\n";
        std::cout<<"Tiền giảm từ Voucher của shop:      "<<this->_voucherDiscount<<"\n";
        std::cout<<"Tiền giảm từ xu:                    "<<this->_coinDiscount<<"\n";
        std::cout<<"SỐ TIỀN CẦN THANH TOÁN:             "<<this->_totalPrice<<"\n";
        std::cout<<"===============================================================\n";
    }
};

#endif