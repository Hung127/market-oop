#ifndef ORDERDTO_H
#define ORDERDTO_H

#include <memory>
#include <string>
#include <vector>

#include "OrderItemDTO.h"

class OrderDTO {
   private:
    std::string _id;
    std::vector<OrderItemDTO> _items;
    double _totalPrice;
    std::string _date;

   public:
    OrderDTO();
    OrderDTO(const std::string& id, const std::string& buyerId,
             const std::vector<OrderItemDTO>& items, double total, const std::string& date);

    // Accessors
    const std::vector<OrderItemDTO>& items() const;
    double totalPrice() const;
    const std::string& date() const;
    const std::string& orderId() const;
    const std::string& buyerId() const;

    // Mutators / helpers
    void setItems(const std::vector<OrderItemDTO>& items);
    void setTotalPrice(double total);
    void setDate(const std::string& date);

    // Convenience
    void addItem(const OrderItemDTO& item);

    std::shared_ptr<OrderItemDTO> findItemByProductId(const std::string& productId);
    void recalculateTotal();
};

#endif  // ORDERDTO_H
