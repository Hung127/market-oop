#ifndef MARKET_H_
#define MARKET_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "./bus/SellerBUS.h"
#include "Product.h"
#include "SearchHelper.h"

class Market {
   private:
    std::map<std::string, std::weak_ptr<Product>> _listedProducts;
    static Market* _instance;

   public:
    // ========== EXISTING METHODS (giữ nguyên) ==========
    std::expected<void, BusError> listProduct(std::shared_ptr<Product> product);
    std::expected<void, BusError> unlistProduct(const std::string& productId);
    void cleanup();

    std::vector<std::shared_ptr<Product>> searchProductsByName(const std::string& keyword) const;
    std::vector<std::shared_ptr<Product>> searchProductsByPriceRange(double minPrice,
                                                                     double maxPrice) const;
    std::shared_ptr<Product> searchClosestProduct(const std::string& keyword) const;
    std::shared_ptr<Product> findProductById(const std::string& productId) const;
    std::vector<std::shared_ptr<Product>> getAllProducts() const;
    size_t getProductCount() const;

    void displayAllProducts() const;

    // ========== SINGLETON ==========
    Market();
    ~Market() = default;
    static Market* getInstance();
};

#endif
