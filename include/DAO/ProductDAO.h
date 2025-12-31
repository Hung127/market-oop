#ifndef PRODUCT_DAO_H
#define PRODUCT_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "../DTO/ProductDTO.h"

class DatabaseManager;

class ProductDAO {
   private:
    static std::shared_ptr<DatabaseManager> _dbManager;

   public:
    static void setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager);

    // Basic CRUD operations
    static std::expected<std::shared_ptr<ProductDTO>, std::string>
    getProductById(const std::string& id);

    static std::vector<std::shared_ptr<ProductDTO>> getAllProducts();

    static std::vector<std::shared_ptr<ProductDTO>>
    getProductsBySeller(const std::string& sellerId);

    static bool insert(const ProductDTO& product);
    static bool insert(const std::shared_ptr<ProductDTO>& product);
    static bool update(const ProductDTO& product);
    static bool remove(const std::string& productId);

    // Additional query methods
    static std::vector<std::shared_ptr<ProductDTO>> searchByName(const std::string& keyword);

    static std::vector<std::shared_ptr<ProductDTO>> getProductsByPriceRange(double minPrice,
                                                                            double maxPrice);

    static bool updateStock(const std::string& productId, int newStock);
    static bool decrementStock(const std::string& productId, int quantity);
    static bool incrementStock(const std::string& productId, int quantity);

    static int getProductCount();
    static int getProductCountBySeller(const std::string& sellerId);

    static bool exists(const std::string& productId);

    // void saveToFile(const std::string& fileName, const ProductExtraInfoDTO& dto);
    // void loadFromFile(const std::string& fileName, ProductExtraInfoDTO& dto);
};

// // Lưu ảnh
//
#endif
