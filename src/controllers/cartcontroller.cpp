#include "cartcontroller.h"
#include "../models/cartmodel.h"
#include "../models/productmodel.h"

CartController::CartController(QObject *parent)
	: QObject(parent)
{
	m_cart = new CartModel(this);
}

CartModel *CartController::cartModel() const
{
	return m_cart;
}

void CartController::addFromProductModel(ProductModel *productModel, int row, int quantity)
{
	if (!productModel)
		return;
	if (row < 0 || row >= productModel->rowCount())
		return;

	QVariant name = productModel->data(productModel->index(row, 0), ProductModel::NameRole);
	QVariant price = productModel->data(productModel->index(row, 0), ProductModel::PriceRole);
	QVariant oldPrice = productModel->data(productModel->index(row, 0), ProductModel::OldPriceRole);
	QVariant discount = productModel->data(productModel->index(row, 0), ProductModel::DiscountRole);
	QVariant rating = productModel->data(productModel->index(row, 0), ProductModel::RatingRole);
	QVariant reviews = productModel->data(productModel->index(row, 0), ProductModel::ReviewCountRole);
	QVariant image = productModel->data(productModel->index(row, 0), ProductModel::ImageUrlRole);

	Product p;
	p.name = name.toString();
	p.price = price.toDouble();
	p.oldPrice = oldPrice.toDouble();
	p.discountPercent = discount.toInt();
	p.rating = rating.toDouble();
	p.reviewCount = reviews.toInt();
	p.imageUrl = image.toString();

	m_cart->addProduct(p, quantity);
}

void CartController::clearCart()
{
	m_cart->clear();
}

