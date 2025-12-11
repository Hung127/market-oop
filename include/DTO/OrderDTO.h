#ifndef ORDERDTO_H
#define ORDERDTO_H

#include <string>
#include <vector>

class OrderItemDTO;

class OrderDTO {
   private:
    std::vector<OrderItemDTO> _items;
    double _totalPrice;
    std::string _date;

   public:
    OrderDTO();
    OrderDTO(const std::vector<OrderItemDTO>& items, double total, const std::string& date);

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
};

#endif  // ORDERDTO_H
