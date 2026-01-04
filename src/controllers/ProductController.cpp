#include "../../include/controllers/ProductController.h"

#include <QDebug>
#include <QVariantMap>

#include "../../include/DAO/ProductDAO.h"
#include "../../include/DTO/ProductDTO.h"
#include "../../include/DTO/SellerDTO.h"

ProductController::ProductController(QObject* parent) : QObject(parent) {
    qDebug() << "[ProductController] Initialized";
}

QVariantList ProductController::getAllProducts() {
    qDebug() << "[ProductController] Loading all products... ";

    auto products = ProductDAO::getAllProducts();

    qDebug() << "[ProductController] Found" << products.size() << "products";

    QVariantList result;
    for (const auto& product : products) {
        if (product) {
            result.append(productToVariant(product));
        }
    }

    emit productsLoaded(result.size());
    return result;
}

QVariantList ProductController::searchProducts(const QString& keyword) {
    qDebug() << "[ProductController] Searching for:" << keyword;

    if (keyword.isEmpty()) {
        return getAllProducts();
    }

    auto products = ProductDAO::searchByName(keyword.toStdString());

    qDebug() << "[ProductController] Found" << products.size() << "matching products";

    QVariantList result;
    for (const auto& product : products) {
        if (product) {
            result.append(productToVariant(product));
        }
    }

    emit searchCompleted(result.size());
    return result;
}

QVariantList ProductController::getProductsByPriceRange(double minPrice, double maxPrice) {
    qDebug() << "[ProductController] Getting products in range:" << minPrice << "-" << maxPrice;

    auto products = ProductDAO::getProductsByPriceRange(minPrice, maxPrice);

    QVariantList result;
    for (const auto& product : products) {
        if (product) {
            result.append(productToVariant(product));
        }
    }

    return result;
}

QVariant ProductController::getProductById(const QString& productId) {
    qDebug() << "[ProductController] Getting product:" << productId;

    auto productResult = ProductDAO::getProductById(productId.toStdString());

    if (productResult.has_value()) {
        return productToVariant(productResult.value());
    }

    qDebug() << "[ProductController] Product not found:" << productId;
    emit productNotFound(productId);
    return QVariant();
}

QVariantList ProductController::getProductsBySeller(const QString& sellerId) {
    qDebug() << "[ProductController] Getting products by seller:" << sellerId;

    auto products = ProductDAO::getProductsBySeller(sellerId.toStdString());

    QVariantList result;
    for (const auto& product : products) {
        if (product) {
            result.append(productToVariant(product));
        }
    }

    return result;
}

int ProductController::getProductCount() {
    return ProductDAO::getProductCount();
}

QVariant ProductController::productToVariant(const std::shared_ptr<ProductDTO>& product) {
    QVariantMap map;

    map["productId"] = QString::fromStdString(product->getID());
    map["name"] = QString::fromStdString(product->getName());
    map["price"] = product->getPrice();
    map["stock"] = product->getStock();
    map["description"] = QString::fromStdString(product->getDescription());
    map["imagePath"] = QString::fromStdString(product->getImagePath());

    // Get seller info
    auto seller = product->getOwner();
    if (seller) {
        map["sellerId"] = QString::fromStdString(seller->getId());
        map["sellerName"] = QString::fromStdString(seller->getName());
    } else {
        map["sellerId"] = "";
        map["sellerName"] = "Unknown";
    }

    // Add emoji based on product name for UI
    QString name = QString::fromStdString(product->getName()).toLower();
    if (name.contains("laptop"))
        map["image"] = "💻";
    else if (name.contains("mouse"))
        map["image"] = "🖱️";
    else if (name.contains("keyboard"))
        map["image"] = "⌨️";
    else if (name.contains("monitor"))
        map["image"] = "🖥️";
    else if (name.contains("phone"))
        map["image"] = "📱";
    else if (name.contains("headphone"))
        map["image"] = "🎧";
    else if (name.contains("watch"))
        map["image"] = "⌨️";
    else if (name.contains("speaker"))
        map["image"] = "🔊";
    else
        map["image"] = "📦";

    return map;
}
