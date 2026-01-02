#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QAbstractListModel>
#include <QString>

struct Category {
	QString name;
	QString icon;
};

class CategoryModel : public QAbstractListModel {
	Q_OBJECT
public:
	enum Roles {
		NameRole = Qt::UserRole + 1,
		IconRole
	};

	explicit CategoryModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QHash<int, QByteArray> roleNames() const override;

	Q_INVOKABLE void addCategory(const QString &name, const QString &icon = QString());

private:
	QList<Category> m_items;
};

#endif // CATEGORYMODEL_H

