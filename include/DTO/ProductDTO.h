#ifndef PRODUCT_DTO_H_
#define PRODUCT_DTO_H_

#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <vector>

using std::string;
using std::vector;

class SellerDTO;

class ProductDTO {
   private:
    std::weak_ptr<SellerDTO> _owner;
    string _sellerId;
    string _id;
    string _description;
    string _name;
    double _price;
    int _stock;
    string _imagePath;

   public:
    // constructor: description and imagePath are optional
    ProductDTO(const string& id, const string& name, double price, int stock,
               const std::shared_ptr<SellerDTO>& owner, const string& description = "NONE",
               const string& imagePath = "NONE");

    // ===== GETTERS (non-const) =====
    string getName();
    string getID();
    double getPrice();
    int getStock();
    std::shared_ptr<SellerDTO> getOwner();

    // ===== GETTERS (const) =====
    string getName() const;
    string getID() const;
    double getPrice() const;
    int getStock() const;
    std::shared_ptr<SellerDTO> getOwner() const;
    string getSellerId() const;
    const std::string& getImagePath() const;
    const std::string& getDescription() const;

    // ===== SETTERS =====
    void setName(const string& name);
    void setPrice(double price);
    void setStock(int stock);
    void setOwner(const std::shared_ptr<SellerDTO>& owner);

    // New setters for description / image path
    void setImagePath(const std::string& path);
    void setDescription(const std::string& desc);
};

#endif
