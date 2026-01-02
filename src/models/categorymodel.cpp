#include "categorymodel.h"

CategoryModel::CategoryModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

int CategoryModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return 0;
	return m_items.size();
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
		return {};

	const Category &c = m_items.at(index.row());
	switch (role) {
	case NameRole: return c.name;
	case IconRole: return c.icon;
	default: return {};
	}
}

QHash<int,QByteArray> CategoryModel::roleNames() const
{
	return {
		{ NameRole, "name" },
		{ IconRole, "icon" }
	};
}

void CategoryModel::addCategory(const QString &name, const QString &icon)
{
	beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
	m_items.append(Category{name, icon});
	endInsertRows();
}

