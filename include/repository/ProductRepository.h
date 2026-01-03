#ifndef PRODUCT_REPOSITORY_H
#define PRODUCT_REPOSITORY_H

#include <expected>
#include <memory>
#include <vector>
#include "../interface/IMarketRepository.h"  // Repository Pattern: Implements interface for abstraction
#include "../DTO/ProductDTO.h"
#include "../database/DatabaseManager.h"

class ProductRepository : public IMarketRepository {  // Repository Pattern: Inherits from interface for polymorphism
private:
    DatabaseManager& _dbManager;  // Dependency Injection: Inject DB manager instead of static

public:
    // Constructor with dependency injection
    explicit ProductRepository(DatabaseManager& dbManager) : _dbManager(dbManager) {}

    // Implement IMarketRepository methods
    std::expected<void, std::string> addProduct(const std::shared_ptr<ProductDTO>& p) override;
    std::expected<void, std::string> removeProduct(const std::string& id) override;
    std::shared_ptr<ProductDTO> findProductById(const std::string& id) const override;
    std::vector<std::shared_ptr<ProductDTO>> searchByName(const std::string& keyword) const override;
    std::vector<std::shared_ptr<ProductDTO>> getAllProducts() const override;
    int getProductCount() const override;
};

#endif