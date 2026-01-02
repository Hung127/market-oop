#include "productcontroller.h"
#include "../models/productmodel.h"

ProductController::ProductController(QObject *parent)
	: QObject(parent)
{
}

QVariantMap ProductController::findByName(ProductModel *model, const QString &name) const
{
	QVariantMap empty;
	if (!model)
		return empty;

	for (int r = 0; r < model->rowCount(); ++r) {
		QModelIndex idx = model->index(r, 0);
		QVariant v = model->data(idx, ProductModel::NameRole);
		if (v.toString() == name) {
			QVariantMap m;
			m.insert("name", model->data(idx, ProductModel::NameRole));
			m.insert("price", model->data(idx, ProductModel::PriceRole));
			m.insert("oldPrice", model->data(idx, ProductModel::OldPriceRole));
			m.insert("discount", model->data(idx, ProductModel::DiscountRole));
			m.insert("rating", model->data(idx, ProductModel::RatingRole));
			m.insert("reviewCount", model->data(idx, ProductModel::ReviewCountRole));
			m.insert("imageUrl", model->data(idx, ProductModel::ImageUrlRole));
			return m;
		}
	}
	return empty;
}

