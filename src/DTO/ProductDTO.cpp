#include "../../include/DTO/ProductDTO.h"

#include <expected>
#include <memory>

#include "../../include/DTO/SellerDTO.h"

//================== PRODUCT_DTO ==================

ProductDTO::ProductDTO(const std::string& id, const std::string& name, double price, int stock,
                       const std::shared_ptr<SellerDTO>& owner)
    : _owner(owner),
      _sellerId(owner ? owner->getId() : ""),
      _id(id),
      _name(name),
      _price(price),
      _stock(stock) {}

// ========== GETTERS (Non-const) ==========
std::string ProductDTO::getName() {
    return _name;
}
std::string ProductDTO::getID() {
    return _id;
}
double ProductDTO::getPrice() {
    return _price;
}
int ProductDTO::getStock() {
    return _stock;
}
std::shared_ptr<SellerDTO> ProductDTO::getOwner() {
    return _owner.lock();
}

std::shared_ptr<ProductExtraInfoDTO> ProductDTO::getExtraInfo() {
    return _extraInfo;
}

// ========== GETTERS (Const) ==========
std::string ProductDTO::getName() const {
    return _name;
}
std::string ProductDTO::getID() const {
    return _id;
}
double ProductDTO::getPrice() const {
    return _price;
}
int ProductDTO::getStock() const {
    return _stock;
}
std::shared_ptr<SellerDTO> ProductDTO::getOwner() const {
    return _owner.lock();
}
std::string ProductDTO::getSellerId() const {
    return _sellerId;
}

// Lưu ý: Dùng shared_ptr<const ...> để đảm bảo tính đúng đắn của hằng số
std::shared_ptr<const ProductExtraInfoDTO> ProductDTO::getExtraInfo() const {
    return _extraInfo;
}

// ========== SETTERS ==========
void ProductDTO::setName(const std::string& name) {
    _name = name;
}
void ProductDTO::setPrice(double price) {
    if (price >= 0)
        _price = price;
}
void ProductDTO::setStock(int stock) {
    if (stock >= 0)
        _stock = stock;
}
void ProductDTO::setOwner(const std::shared_ptr<SellerDTO>& owner) {
    _owner = owner;
    _sellerId = owner ? owner->getId() : "";
}
void ProductDTO::setExtraInfo(const std::shared_ptr<ProductExtraInfoDTO>& extra) {
    _extraInfo = extra;
}

//================== PRODUCT_EXTRA_INFO_DTO ==================

const std::string& ProductExtraInfoDTO::getDescription() const {
    return _description;
}

const std::vector<std::vector<uint8_t>>& ProductExtraInfoDTO::getImageRawData() const {
    return _imageRawData;
}

std::expected<std::vector<uint8_t>, std::string>
ProductExtraInfoDTO::getImageAt(size_t index) const {
    if (index < _imageRawData.size()) {
        return _imageRawData[index];
    }

    return std::unexpected("Index anh khong hop le!");
}

void ProductExtraInfoDTO::setDescription(const std::string& description) {
    _description = description;
}

void ProductExtraInfoDTO::addImageData(const std::vector<uint8_t>& data) {
    if (!data.empty()) {
        _imageRawData.push_back(data);
    }
}

size_t ProductExtraInfoDTO::getImageCount() const {
    return _imageRawData.size();
}
