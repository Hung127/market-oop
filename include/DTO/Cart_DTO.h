#ifndef CART_DTO_H_
#define CART_DTO_H_

#include <memory>
#include <tuple>
#include <vector>

#include "Product_DTO.h"

class CartDto {
   private:
    std::vector<std::tuple<std::weak_ptr<ProductDto>, int>> _items;
    double _totalPrice;

   public:
    CartDto();
    CartDto(const std::vector<std::tuple<std::weak_ptr<ProductDto>, int>>& items);

    // ====== GETTERS / SETTERS ======
    double getTotalPrice() const;
    void setTotalPrice(double total);

    std::vector<std::tuple<std::weak_ptr<ProductDto>, int>>& getItems();
    const std::vector<std::tuple<std::weak_ptr<ProductDto>, int>>& getItems() const;
};

#endif
