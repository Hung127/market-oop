#ifndef CART_DTO_H_
#define CART_DTO_H_

#include <memory>
#include <tuple>
#include <vector>

class ProductDTO;

class CartDTO {
   private:
    std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>> _items;
    double _totalPrice;

   public:
    CartDTO();
    CartDTO(const std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>>& items);

    // ====== GETTERS / SETTERS ======
    double getTotalPrice() const;
    void setTotalPrice(double total);

    std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>>& getItems();
    const std::vector<std::tuple<std::weak_ptr<ProductDTO>, int>>& getItems() const;
};

#endif
