#ifndef PRODUCTMODEL_H
#define PRODUCTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <memory>
#include <vector>

class ProductDTO;

class ProductModel : public QAbstractListModel {
    Q_OBJECT

   public:
    enum ProductRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        PriceRole,
        StockRole,
        ImageRole,
        SellerNameRole,
        SellerIdRole,
        DescriptionRole
    };

    explicit ProductModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Load products from database
    Q_INVOKABLE void loadProducts();
    Q_INVOKABLE void searchProducts(const QString& keyword);
    Q_INVOKABLE void filterByCategory(const QString& category);

   private:
    std::vector<std::shared_ptr<ProductDTO>> m_products;

    void setProducts(const std::vector<std::shared_ptr<ProductDTO>>& products);
};

#endif  // PRODUCTMODEL_H
