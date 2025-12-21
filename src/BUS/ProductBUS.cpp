#include "../../include/BUS/ProductBUS.h"

#include <expected>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "../../include/DTO/ProductDTO.h"

std::expected<void, std::string> ProductBUS::reduceStock(ProductDTO& p, int quantity) {
    if (quantity <= 0) {
        return std::unexpected("Quantity must be positive");
    }
    if (p.getStock() < quantity) {
        return std::unexpected("Not enough stock");
    }

    p.setStock(p.getStock() - quantity);
    return {};
}

void ProductBUS::addStock(ProductDTO& p, int quantity) {
    if (quantity > 0) {
        p.setStock(p.getStock() + quantity);
    }
}

bool ProductBUS::hasEnoughStock(const ProductDTO& p, int quantity) {
    return p.getStock() >= quantity && quantity > 0;
}

bool ProductBUS::isInStock(const ProductDTO& p) {
    return p.getStock() > 0;
}

// ========== DATA UPDATE LOGIC (Validation) ==========

std::expected<void, std::string> ProductBUS::updatePrice(ProductDTO& p, double newPrice) {
    if (newPrice <= 0) {
        return std::unexpected("Price must be greater than 0");
    }

    p.setPrice(newPrice);
    return {};
}

// ========== DISPLAY LOGIC ==========

std::string ProductBUS::toString(const ProductDTO& p) {
    std::stringstream ss;
    ss << "ID: " << std::left << std::setw(6) << p.getID() << "| Name: " << std::left
       << std::setw(20) << p.getName() << "| Price: " << std::fixed << std::setprecision(2)
       << p.getPrice() << "| Stock: " << p.getStock();
    return ss.str();
}

void ProductBUS::display(const ProductDTO& p) {
    std::cout << toString(p) << std::endl;
}

// Lưu file vào database.bin
void ProductBUS::processAndSaveProduct(const std::string& desc,
                                       const std::vector<std::string>& paths) {
    ProductExtraInfoDTO dto(desc);

    for (const auto& path : paths) {
        if (Utils::ImageHelper::isValidImage(path)) {
            auto bytes = Utils::ImageHelper::readImageToBytes(path);
            dto.addImageData(bytes);
        }
    }
    _dao.saveToFile("../data/database.bin", dto);
}