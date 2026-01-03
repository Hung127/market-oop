#ifndef DAO_SELLER_DAO_H
#define DAO_SELLER_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

class SellerDTO;
class DatabaseManager;

class SellerDAO {
   private:
    static std::shared_ptr<DatabaseManager> _dbManager;

   public:
    static void setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager);

    // Save or update seller
    static bool save(const SellerDTO& seller);

    // Get seller by ID
    static std::shared_ptr<SellerDTO> getSellerById(const std::string& id);

    // Get all sellers
    static std::vector<std::shared_ptr<SellerDTO>> getAllSellers();

    // Statistics
    static int getProductCountForSeller(const std::string& sellerId);
    static double getTotalRevenue(const std::string& sellerId);
    static int getTotalSalesCount(const std::string& sellerId);
};

#endif
