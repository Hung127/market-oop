#ifndef PRODUCT_DTO_H_
#define PRODUCT_DTO_H_

#include <memory>
#include <string>

class Seller;

class ProductDTO {
   private:
    std::weak_ptr<Seller> _owner;
    std::string _id;
    std::string _name;
    double _price;
    int _stock;

    std::string _sellerId;

   public:
    ProductDTO(const std::string& id, const std::string& name, double price, int stock,
               const std::shared_ptr<Seller>& owner);

    // ========== GETTERS (Non-const) ==========
    std::string getName();
    std::string getID();
    double getPrice();
    int getStock();
    std::shared_ptr<Seller> getOwner();

    std::string getSellerId() const;
    // ========== GETTERS (Const) ==========
    std::string getName() const;
    std::string getID() const;
    double getPrice() const;
    int getStock() const;
    std::shared_ptr<Seller> getOwner() const;

    // ========== SETTERS ==========
    void setName(const std::string& name);
    void setPrice(double price);
    void setStock(int stock);
    void setOwner(const std::shared_ptr<Seller>& owner);
};

#endif
