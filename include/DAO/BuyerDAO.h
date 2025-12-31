#ifndef DAO_BUYER_DAO_H
#define DAO_BUYER_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

class BuyerDTO;
class DatabaseManager;

class BuyerDAO {
   private:
    static std::shared_ptr<DatabaseManager> _dbManager;

   public:
    static void setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager);

    // Save or update buyer
    static bool save(const BuyerDTO& buyer);

    // Get buyer by ID
    static std::shared_ptr<BuyerDTO> getBuyerById(const std::string& id);

    // Get all buyers
    static std::vector<std::shared_ptr<BuyerDTO>> getAllBuyers();

    // Cart operations
    static bool addToCart(const std::string& buyerId, const std::string& productId, int quantity);
    static bool removeFromCart(const std::string& buyerId, const std::string& productId);
    static bool updateCartQuantity(const std::string& buyerId, const std::string& productId,
                                   int quantity);
    static bool clearCart(const std::string& buyerId);

    // Get cart items for a buyer
    static std::vector<std::pair<std::string, int>> getCartItems(const std::string& buyerId);
};

#endif
