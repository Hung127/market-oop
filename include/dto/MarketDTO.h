#ifndef MARKET_DTO_H_
#define MARKET_DTO_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Product.h"
#include "../SearchHelper.h"
#include "../bus/SellerBUS.h"

class MarketDTO {
   private:
    std::map<std::string, std::weak_ptr<Product>> _listedProducts;

   public:
    bool addProduct(const std::weak_ptr<Product>& product);
    bool removeProduct(const std::string& id);
    std::shared_ptr<Product> findProduct(const std::string& id);
    std::vector<std::weak_ptr<Product>> getAllProducts();
    int countAllProducts();

    MarketDTO();
};

#endif
