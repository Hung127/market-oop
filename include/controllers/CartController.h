#ifndef CARTCONTROLLER_H
#define CARTCONTROLLER_H

#include <QObject>
#include <QString>
#include <memory>

class BuyerDTO;
class ProductDTO;

class CartController : public QObject {
    Q_OBJECT
    Q_PROPERTY(int itemCount READ itemCount NOTIFY cartChanged)
    Q_PROPERTY(double totalPrice READ totalPrice NOTIFY cartChanged)

   public:
    explicit CartController(QObject* parent = nullptr);
    ~CartController() override = default;

    // Set current buyer (called after login)
    void setBuyer(std::shared_ptr<BuyerDTO> buyer);

    // Property getters
    int itemCount() const;
    double totalPrice() const;

    // Invokable methods
    Q_INVOKABLE bool addToCart(const QString& productId, int quantity = 1);
    Q_INVOKABLE bool removeFromCart(const QString& productId);
    Q_INVOKABLE bool updateQuantity(const QString& productId, int quantity);
    Q_INVOKABLE void clearCart();
    Q_INVOKABLE bool checkout();

    // Get cart items as QVariantList for QML
    Q_INVOKABLE QVariantList getCartItems() const;

   signals:
    void cartChanged();
    void itemAdded(const QString& productName);
    void itemRemoved(const QString& productName);
    void checkoutSuccess(const QString& orderId);
    void checkoutFailed(const QString& error);

   private:
    std::shared_ptr<BuyerDTO> m_buyer;

    void loadCartFromDatabase();
};

#endif  // CARTCONTROLLER_H
