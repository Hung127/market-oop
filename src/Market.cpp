#include "../include/Market.h"

#include <climits>
#include <iostream>

Market* Market::_instance = nullptr;

Market::Market() {
    std::cout << "[Market] Initialized (Seller Owns Model)\n";
}

Market* Market::getInstance() {
    if (_instance == nullptr) {
        _instance = new Market();
    }
    return _instance;
}

// ========================================================================
// PRODUCT LISTING MANAGEMENT
// ========================================================================

bool Market::listProduct(std::shared_ptr<Product> product) {
    if (!product) {
        std::cerr << "[Market ERROR] Cannot list null product\n";
        return false;
    }

    std::string id = product->getID();

    auto it = _listedProducts.find(id);
    if (it != _listedProducts.end() && !it->second.expired()) {
        std::cerr << "[Market WARNING] Product " << id << " already listed\n";
        return false;
    }

    _listedProducts[id] = product;

    std::cout << "[Market] ✓ Listed: " << product->getName() << " (ID: " << id << ")\n";

    return true;
}

bool Market::unlistProduct(const std::string& productId) {
    auto it = _listedProducts.find(productId);
    if (it == _listedProducts.end()) {
        std::cerr << "[Market WARNING] Product " << productId << " not found in listings\n";
        return false;
    }

    _listedProducts.erase(it);

    std::cout << "[Market] ✓ Unlisted product ID: " << productId << "\n";
    return true;
}

void Market::cleanup() {
    int cleaned = 0;

    for (auto it = _listedProducts.begin(); it != _listedProducts.end();) {
        if (it->second.expired()) {
            std::cout << "[Market] Cleaning up expired product: " << it->first << "\n";
            it = _listedProducts.erase(it);
            cleaned++;
        } else {
            ++it;
        }
    }

    if (cleaned > 0) {
        std::cout << "[Market] ✓ Cleaned up " << cleaned << " expired products\n";
    }
}

// ========================================================================
// SEARCH & QUERY
// ========================================================================

std::vector<std::shared_ptr<Product>>
Market::searchProductsByName(const std::string& keyword) const {
    std::vector<std::shared_ptr<Product>> results;

    for (const auto& [id, weakPtr] : _listedProducts) {
        if (auto product = weakPtr.lock()) {
            if (SearchHelper::containsIgnoreCase(product->getName(), keyword)) {
                results.push_back(product);
            }
        }
    }

    return results;
}

std::vector<std::shared_ptr<Product>> Market::searchProductsByPriceRange(double minPrice,
                                                                         double maxPrice) const {
    std::vector<std::shared_ptr<Product>> results;

    for (const auto& [id, weakPtr] : _listedProducts) {
        if (auto product = weakPtr.lock()) {
            double price = product->getPrice();
            if (price >= minPrice && price <= maxPrice) {
                results.push_back(product);
            }
        }
    }

    return results;
}

std::shared_ptr<Product> Market::searchClosestProduct(const std::string& keyword) const {
    if (_listedProducts.empty()) {
        return nullptr;
    }

    std::shared_ptr<Product> bestMatch = nullptr;
    int bestScore = INT_MAX;

    for (const auto& [id, weakPtr] : _listedProducts) {
        if (auto product = weakPtr.lock()) {
            int score = SearchHelper::editDistance(SearchHelper::toLower(product->getName()),
                                                   SearchHelper::toLower(keyword));

            if (score < bestScore) {
                bestScore = score;
                bestMatch = product;
            }
        }
    }

    return bestMatch;
}

std::shared_ptr<Product> Market::findProductById(const std::string& productId) const {
    auto it = _listedProducts.find(productId);
    if (it != _listedProducts.end()) {
        return it->second.lock();
    }
    return nullptr;
}

std::vector<std::shared_ptr<Product>> Market::getAllProducts() const {
    std::vector<std::shared_ptr<Product>> results;
    results.reserve(_listedProducts.size());

    for (const auto& [id, weakPtr] : _listedProducts) {
        if (auto product = weakPtr.lock()) {
            results.push_back(product);
        }
    }

    return results;
}

size_t Market::getProductCount() const {
    size_t count = 0;

    for (const auto& [id, weakPtr] : _listedProducts) {
        if (!weakPtr.expired()) {
            count++;
        }
    }

    return count;
}

void Market::displayAllProducts() const {
    std::cout << "\n================= DANH SACH SAN PHAM ==================" << std::endl;

    if (this->_listedProducts.empty()) {
        std::cout << "--- Hien tai khong co san pham nao  ---" << std::endl;
        return;
    }

    // Duyệt qua danh sách sản phẩm và in thông tin
    for (const auto& [id, weakPtr] : this->_listedProducts) {
        // ✅ Lock weak_ptr trước khi dùng
        if (auto product = weakPtr.lock()) {
            // Sử dụng phương thức toString() của đối tượng Product
            std::cout << product->toString() << std::endl;
        }
        // Nếu weak_ptr expired, bỏ qua sản phẩm này
    }

    std::cout << "========================================================\n" << std::endl;
}
