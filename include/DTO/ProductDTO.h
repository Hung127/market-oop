#ifndef PRODUCT_DTO_H_
#define PRODUCT_DTO_H_

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using std::string;
using std::vector;

class Seller;

// ================= PRODUCT EXTRA INFO DTO =================
class ProductExtraInfoDTO {
   private:
    string _description;
    std::vector<std::vector<uint8_t>> _imageRawData;

   public:
    ProductExtraInfoDTO() = default;
    ProductExtraInfoDTO(const string& desc) : _description(desc) {}
    // ===== SETTERS =====
    void setDescription(const std::string& description);
    void addImageData(const std::vector<uint8_t>& data);
    // ===== GETTERS =====
    size_t getImageCount() const;
    const std::vector<uint8_t>& getImageAt(size_t index) const;
    const std::string& getDescription() const;
    const std::vector<std::vector<uint8_t>>& getImageRawData() const;
};

// ================= PRODUCT DTO =================
class ProductDTO {
   private:
    std::weak_ptr<SellerDTO> _owner;
    string _sellerId;
    string _id;
    string _name;
    double _price;
    int _stock;

    std::shared_ptr<ProductExtraInfoDTO> _extraInfo;

   public:
    ProductDTO(const string& id, const string& name, double price, int stock,
               const std::shared_ptr<SellerDTO>& owner);

    // ===== GETTERS (non-const) =====
    string getName();
    string getID();
    double getPrice();
    int getStock();
    std::shared_ptr<SellerDTO> getOwner();

    std::string getSellerId() const;
    // ========== GETTERS (Const) ==========
    std::string getName() const;
    std::string getID() const;
    double getPrice() const;
    int getStock() const;
    std::shared_ptr<SellerDTO> getOwner() const;

    // ===== SETTERS =====
    void setName(const string& name);
    void setPrice(double price);
    void setStock(int stock);
    void setOwner(const std::shared_ptr<SellerDTO>& owner);

    // Extra info
    std::shared_ptr<ProductExtraInfoDTO> getExtraInfo();
    std::shared_ptr<const ProductExtraInfoDTO> getExtraInfo() const;
    void setExtraInfo(const std::shared_ptr<ProductExtraInfoDTO>& extra);
};

#endif