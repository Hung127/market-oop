#ifndef PRODUCTCONTROLLER_H
#define PRODUCTCONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantList>
#include <memory>

class ProductDTO;

class ProductController : public QObject {
    Q_OBJECT

   public:
    explicit ProductController(QObject* parent = nullptr);

    // Methods callable from QML
    Q_INVOKABLE QVariantList getAllProducts();
    Q_INVOKABLE QVariantList searchProducts(const QString& keyword);
    Q_INVOKABLE QVariantList getProductsByPriceRange(double minPrice, double maxPrice);
    Q_INVOKABLE QVariant getProductById(const QString& productId);
    Q_INVOKABLE QVariantList getProductsBySeller(const QString& sellerId);
    Q_INVOKABLE int getProductCount();

   signals:
    void productsLoaded(int count);
    void searchCompleted(int count);
    void productNotFound(const QString& productId);

   private:
    QVariant productToVariant(const std::shared_ptr<ProductDTO>& product);
};

#endif  // PRODUCTCONTROLLER_H
