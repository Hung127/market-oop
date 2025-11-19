#ifndef PRODUCT_H_
#define PRODUCT_H_

#include <expected>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

class Seller;  // Forward declaration

class Product {
   private:
    std::weak_ptr<Seller> _owner;
    std::string _id;
    std::string _name;
    double _price;
    int _stock;

   public:
    Product(const std::string& id, const std::string& name, double price, int stock,
            const std::shared_ptr<Seller>& owner);

    // ========== GETTERS (Non-const) ==========
    std::string getName();
    std::string getID();
    double getPrice();
    int getStock();
    std::shared_ptr<Seller> getOwner();

    // ========== GETTERS (Const) ==========
    std::string getName() const;
    std::string getID() const;
    double getPrice() const;
    int getStock() const;
    std::shared_ptr<Seller> getOwner() const;

    // ========== SETTERS ==========
    void setName(const std::string& name) {
        _name = name;
    }
    void setPrice(double price) {
        if (price > 0) {
            _price = price;
        }
    }
    void setStock(int stock) {
        if (stock >= 0) {
            _stock = stock;
        }
    }

    // ========== STOCK MANAGEMENT ==========
    std::expected<void, std::string> reduceStock(int quantity);
    void addStock(int quantity);
    bool isInStock() const {
        return _stock > 0;
    }
    bool hasEnoughStock(int quantity) const;
    bool hasEnoughStock(int quantity);

    // ========== OWNER MANAGEMENT ==========
    void setOwner(const std::shared_ptr<Seller>& owner);
    bool hasOwner() const;

    // ========== DISPLAY ✅ THÊM MỚI ==========

    /**
     * @brief Chuyển thông tin sản phẩm thành chuỗi để in ra
     *
     * Format: "ID: P001  | Name: Laptop Dell XPS    | Price: 1299.99 | Stock:
     * 10"
     *
     * @return Chuỗi đã format
     */
    std::string toString() const;

    /**
     * @brief In thông tin sản phẩm ra stdout
     */
    void display() const;
};

#endif
