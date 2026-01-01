#include "productmodel.h"

ProductModel::ProductModel(QObject *parent)
    : QAbstractListModel(parent)
{}

int ProductModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant ProductModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const Product &p = m_items.at(index.row());
    switch (role) {
    case NameRole:        return p.name;
    case PriceRole:       return p.price;
    case OldPriceRole:    return p.oldPrice;
    case DiscountRole:    return p.discountPercent;
    case RatingRole:      return p.rating;
    case ReviewCountRole: return p.reviewCount;
    case ImageUrlRole:    return p.imageUrl;
    default:              return {};
    }
}

QHash<int, QByteArray> ProductModel::roleNames() const
{
    return {
        { NameRole,        "name" },
        { PriceRole,       "price" },
        { OldPriceRole,    "oldPrice" },
        { DiscountRole,    "discount" },
        { RatingRole,      "rating" },
        { ReviewCountRole, "reviewCount" },
        { ImageUrlRole,    "imageUrl" }
    };
}

void ProductModel::addProduct(const Product &p)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.append(p);
    endInsertRows();
}