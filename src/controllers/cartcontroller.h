#ifndef CARTCONTROLLER_H
#define CARTCONTROLLER_H

#include <QObject>

class CartModel;
class ProductModel;

class CartController : public QObject {
	Q_OBJECT
public:
	explicit CartController(QObject *parent = nullptr);

	Q_INVOKABLE CartModel *cartModel() const;
	Q_INVOKABLE void addFromProductModel(ProductModel *productModel, int row, int quantity = 1);
	Q_INVOKABLE void clearCart();

private:
	CartModel *m_cart;
};

#endif // CARTCONTROLLER_H

