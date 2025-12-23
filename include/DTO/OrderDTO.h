#ifndef ORDERDTO_H
#define ORDERDTO_H

#include <string>
#include <vector>

#include "OrderItemDTO.h"

class OrderDTO {
   private:
    std::vector<OrderItemDTO> _items;
    double _totalPrice;
    std::string _date;

    double _rawTotal;        // Tổng tiền gốc (chưa giảm)
    double _voucherDiscount; // Số tiền giảm từ Voucher
    double _coinDiscount;    // Số tiền giảm từ Xu
    double _totalPrice;      // Số tiền thực thanh toán cuối cùng

   public:
    OrderDTO();
    OrderDTO(const std::vector<OrderItemDTO>& items, double rawTotal, const std::string& date);

    // Accessors
    const std::vector<OrderItemDTO>& items() const;
    double totalPrice() const;
    const std::string& date() const;

    // Mutators / helpers
    void setItems(const std::vector<OrderItemDTO>& items);
    void setTotalPrice(double total);
    void setDate(const std::string& date);

    // Convenience
    void addItem(const OrderItemDTO& item);
    void recalculateTotal();  // optional helper if you want to compute total from items

    void setDiscounts(double vDiscount, double cDiscount);
    double getRawTotal() const;
    double getVoucherDiscount() const ;
    double getCoinDiscount() const;
    double totalPrice() const ;
    void setTotalPrice(double price);
};

#endif  // ORDERDTO_H
