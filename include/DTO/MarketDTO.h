#ifndef MARKET_DTO_H_
#define MARKET_DTO_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

class ProductDTO;

class MarketDTO {
   private:
    std::map<std::string, std::weak_ptr<ProductDTO>> _listedProducts;

   public:
    bool addProduct(const std::shared_ptr<ProductDTO>& product);
    bool removeProduct(const std::string& id);
    [[nodiscard]] std::shared_ptr<ProductDTO> findProduct(const std::string& id) const;
    [[nodiscard]] std::vector<std::shared_ptr<ProductDTO>> getAllProducts() const;
    [[nodiscard]] int countAllProducts() const;
    void cleanupExpired();

    MarketDTO();
};

#endif
