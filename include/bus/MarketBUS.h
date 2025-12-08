#ifndef MARKET_BUS_H_
#define MARKET_BUS_H_

#include <expected>
#include <memory>
#include <string>

#include "../interface/IMarketRepository.h"

enum class MarketBusError { ValidationFailed, NotFound, Conflict, RepositoryError, InternalError };

static inline std::string marketBusErrorString(MarketBusError e) {
    switch (e) {
        case MarketBusError::ValidationFailed:
            return "ValidationFailed";
        case MarketBusError::NotFound:
            return "NotFound";
        case MarketBusError::Conflict:
            return "Conflict";
        case MarketBusError::RepositoryError:
            return "RepositoryError";
        default:
            return "InternalError";
    }
}

class MarketBUS {
   private:
    std::shared_ptr<IMarketRepository> _repo;

   public:
    std::expected<void, MarketBusError> addProduct(const std::shared_ptr<Product>& p);
    std::expected<void, MarketBusError> removeProduct(const std::string& id);
    std::shared_ptr<Product> findProductById(const std::string& id) const;
    std::vector<std::shared_ptr<Product>> searchByName(const std::string& keyword) const;
    std::vector<std::shared_ptr<Product>> getAllProducts() const;
    int getProductCount() const;

    MarketBUS(const std::shared_ptr<IMarketRepository>& repo);
};

#endif
