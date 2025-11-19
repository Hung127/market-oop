#include "../include/Seller.h"

#include <algorithm>
#include <climits>
#include <iomanip>
#include <iostream>
#include <memory>

#include "../include/Market.h"
#include "../include/Product.h"
#include "../include/SearchHelper.h"

// ========================================================================
// PRODUCT MANAGEMENT
// ========================================================================

std::shared_ptr<Product> Seller::createProduct(const std::string& id, const std::string& name,
                                               double price, int stock) {
    // Validate inputs
    if (id.empty() || name.empty()) {
        std::cerr << "[Seller ERROR] ID and name cannot be empty\n";
        return nullptr;
    }

    if (price <= 0) {
        std::cerr << "[Seller ERROR] Price must be positive\n";
        return nullptr;
    }

    if (stock < 0) {
        std::cerr << "[Seller ERROR] Stock cannot be negative\n";
        return nullptr;
    }

    // Check duplicate ID
    if (findProductInInventory(id)) {
        std::cerr << "[Seller ERROR] Product ID '" << id << "' already exists in inventory\n";
        return nullptr;
    }

    // Create product (owner is nullptr for now, can be set later if needed)
    auto product = std::make_shared<Product>(id, name, price, stock, nullptr);
    _myProducts.push_back(product);

    std::cout << "[Seller] ✓ Created product: " << name << " (ID: " << id << ") by " << getName()
              << "\n";

    return product;
}

bool Seller::deleteProduct(Market& market, const std::string& productId) {
    auto it = std::find_if(
        _myProducts.begin(), _myProducts.end(),
        [&productId](const std::shared_ptr<Product>& p) { return p->getID() == productId; });

    if (it == _myProducts.end()) {
        std::cerr << "[Seller ERROR] Product '" << productId << "' not found in inventory\n";
        return false;
    }

    // Unpublish from market first (if published)
    market.unlistProduct(productId);

    // Remove from inventory
    std::string productName = (*it)->getName();
    _myProducts.erase(it);

    std::cout << "[Seller] ✓ Deleted product: " << productName << " (ID: " << productId << ")\n";

    return true;
}

bool Seller::updateProduct(const std::string& productId, const std::string& newName,
                           double newPrice) {
    auto product = findProductInInventory(productId);
    if (!product) {
        std::cerr << "[Seller ERROR] Product '" << productId << "' not found\n";
        return false;
    }

    // Validate price
    if (newPrice != -1 && newPrice <= 0) {
        std::cerr << "[Seller ERROR] Price must be positive\n";
        return false;
    }

    // ✅ UPDATE IN-PLACE (không tạo mới)
    if (!newName.empty()) {
        product->setName(newName);
    }

    if (newPrice != -1) {
        product->setPrice(newPrice);
    }

    std::cout << "[Seller] ✓ Updated product: " << productId << "\n";
    return true;
}

bool Seller::updateStock(const std::string& productId, int newStock) {
    auto product = findProductInInventory(productId);
    if (!product) {
        std::cerr << "[Seller ERROR] Product '" << productId << "' not found\n";
        return false;
    }

    if (newStock < 0) {
        std::cerr << "[Seller ERROR] Stock cannot be negative\n";
        return false;
    }

    // ✅ UPDATE IN-PLACE
    product->setStock(newStock);

    std::cout << "[Seller] ✓ Updated stock for " << productId << " to " << newStock << "\n";
    return true;
}

std::vector<std::shared_ptr<Product>> Seller::getMyProducts() const {
    return _myProducts;
}

// ========================================================================
// MARKET INTERACTION
// ========================================================================

bool Seller::publishToMarket(Market& market, const std::string& productId) {
    auto product = findProductInInventory(productId);
    if (!product) {
        std::cerr << "[Seller ERROR] Product '" << productId << "' not in inventory\n";
        return false;
    }

    if (product->getStock() <= 0) {
        std::cerr << "[Seller WARNING] Product '" << product->getName()
                  << "' has no stock, but publishing anyway\n";
    }

    return market.listProduct(product);
}

bool Seller::unpublishFromMarket(Market& market, const std::string& productId) {
    // Check if product exists in inventory
    auto product = findProductInInventory(productId);
    if (!product) {
        std::cerr << "[Seller ERROR] Product '" << productId << "' not in inventory\n";
        return false;
    }

    return market.unlistProduct(productId);
}

// ========================================================================
// SEARCH IN INVENTORY
// ========================================================================

std::vector<std::shared_ptr<Product>>
Seller::searchMyProductsByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<Product>> results;

    for (const auto& product : _myProducts) {
        if (SearchHelper::containsIgnoreCase(product->getName(), keyword)) {
            results.push_back(product);
        }
    }

    return results;
}

std::shared_ptr<Product> Seller::searchMyClosestProduct(const std::string& keyword) const {
    if (_myProducts.empty()) {
        return nullptr;
    }

    std::shared_ptr<Product> bestMatch = nullptr;
    int bestScore = INT_MAX;

    for (const auto& product : _myProducts) {
        int score = SearchHelper::editDistance(SearchHelper::toLower(product->getName()),
                                               SearchHelper::toLower(keyword));

        if (score < bestScore) {
            bestScore = score;
            bestMatch = product;
        }
    }

    return bestMatch;
}

// ========================================================================
// DISPLAY
// ========================================================================

void Seller::printMyProducts() const {
    if (_myProducts.empty()) {
        std::cout << "\n[Seller] " << getName() << " has no products in inventory.\n";
        return;
    }

    std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Inventory of " << std::left << std::setw(44) << getName() << "║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "Total: " << _myProducts.size() << " product(s)\n\n";

    // ✅ SỬ DỤNG toString() thay vì format thủ công
    for (const auto& product : _myProducts) {
        std::cout << "  " << product->toString() << "\n";
    }

    std::cout << "\n";
}

// ========================================================================
// PRIVATE HELPER
// ========================================================================

std::shared_ptr<Product> Seller::findProductInInventory(const std::string& id) {
    for (auto& p : _myProducts) {
        if (p->getID() == id) {
            return p;
        }
    }
    return nullptr;
}

std::expected<std::unique_ptr<Seller>, std::string> Seller::create(const std::string& id,
                                                                   const std::string& name,
                                                                   const std::string& email,
                                                                   const std::string& password) {
    auto* result = new Seller(id, name, email, password);

    return std::unique_ptr<Seller>(result);
}

Seller::Seller(const std::string& id, const std::string& name, const std::string& email,
               const std::string& password)
    : User(id, name, email, password) {
    // Nothing
}
