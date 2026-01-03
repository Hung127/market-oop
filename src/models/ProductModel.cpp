#include "../../include/models/ProductModel.h"

#include <QDebug>

#include "../../include/DAO/ProductDAO.h"
#include "../../include/DTO/ProductDTO.h"
#include "../../include/DTO/SellerDTO.h"

ProductModel::ProductModel(QObject* parent) : QAbstractListModel(parent) {
    qDebug() << "[ProductModel] Initialized";
}

int ProductModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_products.size());
}

QVariant ProductModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_products.size()))
        return QVariant();

    const auto& product = m_products[index.row()];
    if (!product)
        return QVariant();

    switch (role) {
        case IdRole:
            return QString::fromStdString(product->getID());
        case NameRole:
            return QString::fromStdString(product->getName());
        case PriceRole:
            return product->getPrice();
        case StockRole:
            return product->getStock();
        case ImageRole: {
            // Return emoji based on product name
            QString name = QString::fromStdString(product->getName()).toLower();
            if (name.contains("laptop"))
                return "💻";
            if (name.contains("mouse"))
                return "🖱️";
            if (name.contains("keyboard"))
                return "⌨️";
            if (name.contains("monitor"))
                return "🖥️";
            if (name.contains("phone"))
                return "📱";
            if (name.contains("headphone"))
                return "🎧";
            if (name.contains("camera"))
                return "📷";
            if (name.contains("watch"))
                return "⌚";
            if (name.contains("tablet"))
                return "📱";
            if (name.contains("speaker"))
                return "🔊";
            if (name.contains("printer"))
                return "🖨️";
            if (name.contains("router"))
                return "📡";
            if (name.contains("drive") || name.contains("ssd") || name.contains("hdd"))
                return "💾";
            if (name.contains("cable"))
                return "🔌";
            if (name.contains("charger"))
                return "🔋";
            return "📦";  // Default
        }
        case SellerIdRole:
            return QString::fromStdString(product->getSellerId());
        case SellerNameRole: {
            auto seller = product->getOwner();
            if (seller) {
                return QString::fromStdString(seller->getName());
            }
            return "Unknown Seller";
        }
        case DescriptionRole:
            return QString::fromStdString(product->getDescription());
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> ProductModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[IdRole] = "productId";
    roles[NameRole] = "name";
    roles[PriceRole] = "price";
    roles[StockRole] = "stock";
    roles[ImageRole] = "image";
    roles[SellerNameRole] = "seller";
    roles[SellerIdRole] = "sellerId";
    roles[DescriptionRole] = "description";
    return roles;
}

void ProductModel::loadProducts() {
    qDebug() << "[ProductModel] Loading all products... ";

    // Use ProductDAO to get all products
    auto products = ProductDAO::getAllProducts();

    qDebug() << "[ProductModel] Loaded" << products.size() << "products";

    // Debug: Print product details
    for (const auto& product : products) {
        if (product) {
            qDebug() << "  - ID:" << QString::fromStdString(product->getID())
                     << "Name:" << QString::fromStdString(product->getName())
                     << "Price:" << product->getPrice() << "Stock:" << product->getStock();
        }
    }

    setProducts(products);
}

void ProductModel::searchProducts(const QString& keyword) {
    qDebug() << "[ProductModel] Searching for:" << keyword;

    if (keyword.isEmpty()) {
        loadProducts();
        return;
    }

    // Use ProductDAO to search by name
    auto products = ProductDAO::searchByName(keyword.toStdString());

    qDebug() << "[ProductModel] Found" << products.size() << "products";

    setProducts(products);
}

void ProductModel::filterByCategory(const QString& category) {
    qDebug() << "[ProductModel] Filtering by category:" << category;

    if (category == "All Products" || category.isEmpty()) {
        loadProducts();
        return;
    }

    // For now, just do a simple name search by category
    // In a real app, you'd have a category field in the database
    auto products = ProductDAO::searchByName(category.toStdString());

    qDebug() << "[ProductModel] Found" << products.size() << "products in category";

    setProducts(products);
}

void ProductModel::refresh() {
    qDebug() << "[ProductModel] Refreshing products... ";
    loadProducts();
}

void ProductModel::setProducts(const std::vector<std::shared_ptr<ProductDTO>>& products) {
    beginResetModel();
    m_products = products;
    endResetModel();

    qDebug() << "[ProductModel] Model updated with" << m_products.size() << "products";
}
