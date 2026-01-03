#include "../../include/models/ProductModel.h"

#include <QDebug>

#include "../../include/DAO/ProductDAO.h"
#include "../../include/DAO/UserDAO.h"
#include "../../include/DTO/ProductDTO.h"
#include "../../include/DTO/SellerDTO.h"

ProductModel::ProductModel(QObject* parent) : QAbstractListModel(parent) {
    // Constructor
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
            // Return emoji based on product name (for now)
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

    auto products = ProductDAO::getAllProducts();

    qDebug() << "[ProductModel] Loaded" << products.size() << "products";

    setProducts(products);
}

void ProductModel::searchProducts(const QString& keyword) {
    qDebug() << "[ProductModel] Searching for:" << keyword;

    if (keyword.isEmpty()) {
        loadProducts();
        return;
    }

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

    setProducts(products);
}

void ProductModel::setProducts(const std::vector<std::shared_ptr<ProductDTO>>& products) {
    beginResetModel();
    m_products = products;
    endResetModel();

    qDebug() << "[ProductModel] Model updated with" << m_products.size() << "products";
}
