#include "cartmodel.h"

CartModel::CartModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

int CartModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_items.size();
}

QVariant CartModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
		return {};

	const CartItem &it = m_items.at(index.row());
	switch (role) {
	case NameRole: return it.product.name;
	case PriceRole: return it.product.price;
	case QuantityRole: return it.quantity;
	case ImageUrlRole: return it.product.imageUrl;
	case TotalRole: return it.product.price * it.quantity;
	default: return {};
	}
}

QHash<int, QByteArray> CartModel::roleNames() const
{
	return {
		{ NameRole, "name" },
		{ PriceRole, "price" },
		{ QuantityRole, "quantity" },
		{ ImageUrlRole, "imageUrl" },
		{ TotalRole, "total" }
	};
}

void CartModel::addProduct(const Product &p, int quantity)
{
	// If product already exists, increase quantity
	for (int i = 0; i < m_items.size(); ++i) {
		if (m_items.at(i).product.name == p.name) {
			m_items[i].quantity += quantity;
			emit dataChanged(index(i, 0), index(i, 0));
			return;
		}
	}

	beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
	CartItem it;
	it.product = p;
	it.quantity = quantity;
	m_items.append(it);
	endInsertRows();
}

void CartModel::removeAt(int index)
{
	if (index < 0 || index >= m_items.size())
		return;
	beginRemoveRows(QModelIndex(), index, index);
	m_items.removeAt(index);
	endRemoveRows();
}

void CartModel::clear()
{
	if (m_items.isEmpty())
		return;
	beginRemoveRows(QModelIndex(), 0, m_items.size() - 1);
	m_items.clear();
	endRemoveRows();
}

double CartModel::totalPrice() const
{
	double s = 0.0;
	for (const CartItem &it : m_items)
		s += it.product.price * it.quantity;
	return s;
}

