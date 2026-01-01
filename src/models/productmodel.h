#ifndef PRODUCTMODEL_H
#define PRODUCTMODEL_H

#include <QAbstractListModel>
#include <QString>

struct Product {
    QString name;
    double price;
    double oldPrice;
    int    discountPercent; // 0 nếu không có
    double rating;          // 0–5
    int    reviewCount;
    QString imageUrl;       // qrc hoặc file path
};

class ProductModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PriceRole,
        OldPriceRole,
        DiscountRole,
        RatingRole,
        ReviewCountRole,
        ImageUrlRole
    };

    explicit ProductModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addProduct(const Product &p);

private:
    QList<Product> m_items;
};

#endif