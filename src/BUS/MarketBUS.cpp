#include "../../include/BUS/MarketBUS.h"

#include <expected>
#include <memory>

std::expected<void, MarketBusError> MarketBUS::addProduct(const std::shared_ptr<Product>& p) {
    if (!p) {
        return std::unexpected(MarketBusError::ValidationFailed);
    }

    std::expected<void, std::string> addProductPack = this->_repo->addProduct(p);
    if (!addProductPack.has_value()) {
        return std::unexpected(MarketBusError::RepositoryError);
    }

    return {};
}

std::expected<void, MarketBusError> MarketBUS::removeProduct(const std::string& id) {
    if (id.empty()) {
        return std::unexpected(MarketBusError::ValidationFailed);
    }

    std::expected<void, std::string> removeProductPack = this->_repo->removeProduct(id);
    if (!removeProductPack.has_value()) {
        return std::unexpected(MarketBusError::RepositoryError);
    }

    return {};
}

std::shared_ptr<Product> MarketBUS::findProductById(const std::string& id) const {
    if (id.empty()) {
        return nullptr;
    }

    return this->_repo->findProductById(id);
}

std::vector<std::shared_ptr<Product>> MarketBUS::searchByName(const std::string& keyword) const {
    if (keyword.empty()) {
        return {};
    }

    return this->_repo->searchByName(keyword);
}

std::vector<std::shared_ptr<Product>> MarketBUS::getAllProducts() const {
    return this->_repo->getAllProducts();
}

int MarketBUS::getProductCount() const {
    return this->_repo->getProductCount();
}

MarketBUS::MarketBUS(const std::shared_ptr<IMarketRepository>& repo) : _repo(repo) {}
