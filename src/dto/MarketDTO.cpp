#include "../../include/dto/MarketDTO.h"

bool MarketDTO::addProduct(const std::weak_ptr<Product>& product) {}

bool MarketDTO::removeProduct(const std::string& id) {}

std::shared_ptr<Product> MarketDTO::findProduct(const std::string& id) {}

std::vector<std::weak_ptr<Product>> MarketDTO::getAllProducts() {}

int countAllProducts() {}

MarketDTO::MarketDTO() {
    // Do nothing
}
