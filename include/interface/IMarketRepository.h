#ifndef IMARKET_REPOSITORY_H_
#define IMARKET_REPOSITORY_H_

#include <expected>
#include <memory>
#include <vector>

class ProductDTO;

class IMarketRepository {
   public:
    virtual ~IMarketRepository() = default;
    virtual std::expected<void, std::string> addProduct(const std::shared_ptr<ProductDTO>& p) = 0;
    virtual std::expected<void, std::string> removeProduct(const std::string& id) = 0;
    virtual std::shared_ptr<ProductDTO> findProductById(const std::string& id) const = 0;
    virtual std::vector<std::shared_ptr<ProductDTO>>
    searchByName(const std::string& keyword) const = 0;
    virtual std::vector<std::shared_ptr<ProductDTO>> getAllProducts() const = 0;
    virtual int getProductCount() const = 0;
};

#endif
