#ifndef PRODUCTCONTROLLER_H
#define PRODUCTCONTROLLER_H

#include <QObject>
#include <QVariantList>

class ProductModel;

class ProductController : public QObject {
	Q_OBJECT
public:
	explicit ProductController(QObject *parent = nullptr);

	// Tìm sản phẩm theo tên trong một ProductModel, trả về QVariantMap có các trường role
	Q_INVOKABLE QVariantMap findByName(ProductModel *model, const QString &name) const;
};

#endif // PRODUCTCONTROLLER_H

