#include "../../include/BUS/ProductBUS.h"

#include <expected>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "../../include/DAO/ProductDAO.h"
#include "../../include/DTO/ProductDTO.h"
#include "../../include/Utils/Utils.h"

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

std::expected<void, std::string> ProductBUS::updatePrice(ProductDTO& p, double newPrice) {
    if (newPrice <= 0) {
        return std::unexpected("Price must be greater than 0");
    }

    p.setPrice(newPrice);

    // persist
    if (!ProductDAO::update(p)) {
        return std::unexpected("Failed to persist updated price");
    }
    return {};
}

// Set image path after validation and persist
std::expected<void, std::string> ProductBUS::setImagePath(ProductDTO& p, const std::string& path) {
    if (path.empty()) {
        return std::unexpected("Image path is empty");
    }
    if (!Utils::ImageHelper::isValidImage(path)) {
        return std::unexpected("Unsupported image extension (allowed: .jpg, .jpeg, .png)");
    }

    // Optionally: verify file exists / readable
    auto bytes = Utils::ImageHelper::readImageToBytes(path);
    if (bytes.empty()) {
        // allow non-existing path? decide policy: here we require readable file
        return std::unexpected("Image file not found or unreadable at path: " + path);
    }

    p.setImagePath(path);

    if (!ProductDAO::update(p)) {
        return std::unexpected("Failed to persist image path");
    }
    return {};
}

std::expected<void, std::string> ProductBUS::clearImagePath(ProductDTO& p) {
    p.setImagePath("");
    if (!ProductDAO::update(p)) {
        return std::unexpected("Failed to persist image clear");
    }
    return {};
}

std::string ProductBUS::toString(const ProductDTO& p) {
    std::stringstream ss;
    ss << "ID: " << std::left << std::setw(6) << p.getID() << "| Name: " << std::left
       << std::setw(20) << p.getName() << "| Price: " << std::fixed << std::setprecision(2)
       << p.getPrice() << "| Stock: " << p.getStock();
    if (!p.getImagePath().empty()) {
        ss << " | Image: " << p.getImagePath();
    }
    return ss.str();
}

void ProductBUS::display(const ProductDTO& p) {
    std::cout << toString(p) << std::endl;
}
