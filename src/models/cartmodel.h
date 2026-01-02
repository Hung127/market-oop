#ifndef CARTMODEL_H
#define CARTMODEL_H

#include <QAbstractListModel>
#include <QString>
#include "productmodel.h"

struct CartItem {
	Product product;
	int quantity{1};
};

class CartModel : public QAbstractListModel {
	Q_OBJECT
public:
	enum Roles {
		NameRole = Qt::UserRole + 1,
		PriceRole,
		QuantityRole,
		ImageUrlRole,
		TotalRole
	};

	explicit CartModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	Q_INVOKABLE void addProduct(const Product &p, int quantity = 1);
	Q_INVOKABLE void removeAt(int index);
	Q_INVOKABLE void clear();
	Q_INVOKABLE double totalPrice() const;

private:
	QList<CartItem> m_items;
};

#endif // CARTMODEL_H

