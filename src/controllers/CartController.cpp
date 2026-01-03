#include "../../include/controllers/CartController.h"

#include <QDebug>
#include <QVariantList>
#include <QVariantMap>

#include "../../include/BUS/BuyerBUS.h"
#include "../../include/BUS/CartBUS.h"
#include "../../include/DAO/BuyerDAO.h"
#include "../../include/DAO/ProductDAO.h"
#include "../../include/DTO/BuyerDTO.h"
#include "../../include/DTO/CartDTO.h"
#include "../../include/DTO/ProductDTO.h"

CartController::CartController(QObject* parent) : QObject(parent), m_buyer(nullptr) {
    qDebug() << "[CartController] Initialized";
}

void CartController::setBuyer(std::shared_ptr<BuyerDTO> buyer) {
    m_buyer = buyer;
    if (m_buyer) {
        qDebug() << "[CartController] Buyer set:" << QString::fromStdString(m_buyer->getName());
        loadCartFromDatabase();
        emit cartChanged();
    }
}

void CartController::loadCartFromDatabase() {
    if (!m_buyer) {
        return;
    }

    qDebug() << "[CartController] Loading cart from database... ";

    // Get cart items from database
    auto cartItems = BuyerDAO::getCartItems(m_buyer->getId());

    qDebug() << "[CartController] Found" << cartItems.size() << "items in cart";

    // Clear current cart
    m_buyer->getCart().getItems().clear();

    // Load each item
    for (const auto& [productId, quantity] : cartItems) {
        auto productResult = ProductDAO::getProductById(productId);
        if (productResult.has_value()) {
            auto product = productResult.value();
            m_buyer->getCart().getItems().emplace_back(product, quantity);
            qDebug() << "  - Loaded:" << QString::fromStdString(product->getName()) << "x"
                     << quantity;
        }
    }

    // Recalculate total
    CartBUS::recalculateTotal(m_buyer->getCart());
}

int CartController::itemCount() const {
    if (!m_buyer) {
        return 0;
    }

    return static_cast<int>(m_buyer->getCart().getItems().size());
}

double CartController::totalPrice() const {
    if (!m_buyer) {
        return 0.0;
    }

    return m_buyer->getCart().getTotalPrice();
}

bool CartController::addToCart(const QString& productId, int quantity) {
    if (!m_buyer) {
        qDebug() << "[CartController] Error: No buyer set";
        return false;
    }

    qDebug() << "[CartController] Adding to cart:" << productId << "x" << quantity;

    // Get product from database
    auto productResult = ProductDAO::getProductById(productId.toStdString());
    if (!productResult.has_value()) {
        qDebug() << "[CartController] Error: Product not found";
        return false;
    }

    auto product = productResult.value();

    // Add to cart using BuyerBUS
    auto result = BuyerBUS::addToCart(*m_buyer, product, quantity);

    if (result.has_value()) {
        // Save to database
        BuyerDAO::addToCart(m_buyer->getId(), productId.toStdString(), quantity);

        qDebug() << "[CartController] ✓ Added to cart";
        emit cartChanged();
        emit itemAdded(QString::fromStdString(product->getName()));
        return true;
    } else {
        qDebug() << "[CartController] ✗ Failed:" << QString::fromStdString(result.error());
        return false;
    }
}

bool CartController::removeFromCart(const QString& productId) {
    if (!m_buyer) {
        return false;
    }

    qDebug() << "[CartController] Removing from cart:" << productId;

    // Remove using BuyerBUS
    bool success = BuyerBUS::removeFromCart(*m_buyer, productId.toStdString());

    if (success) {
        // Remove from database
        BuyerDAO::removeFromCart(m_buyer->getId(), productId.toStdString());

        qDebug() << "[CartController] ✓ Removed from cart";
        emit cartChanged();
        return true;
    }

    return false;
}

bool CartController::updateQuantity(const QString& productId, int quantity) {
    if (!m_buyer) {
        return false;
    }

    qDebug() << "[CartController] Updating quantity:" << productId << "=" << quantity;

    if (quantity <= 0) {
        return removeFromCart(productId);
    }

    // Update in database
    bool success =
        BuyerDAO::updateCartQuantity(m_buyer->getId(), productId.toStdString(), quantity);

    if (success) {
        // Reload cart
        loadCartFromDatabase();
        emit cartChanged();
        return true;
    }

    return false;
}

void CartController::clearCart() {
    if (!m_buyer) {
        return;
    }

    qDebug() << "[CartController] Clearing cart";

    BuyerBUS::clearCart(*m_buyer);
    BuyerDAO::clearCart(m_buyer->getId());

    emit cartChanged();
}

bool CartController::checkout() {
    if (!m_buyer) {
        qDebug() << "[CartController] Error: No buyer set";
        emit checkoutFailed("No buyer logged in");
        return false;
    }

    qDebug() << "[CartController] Processing checkout...";

    // Use BuyerBUS to checkout
    auto result = BuyerBUS::checkout(*m_buyer);

    if (result.has_value()) {
        qDebug() << "[CartController] ✓ Checkout successful";

        // Clear cart in database
        BuyerDAO::clearCart(m_buyer->getId());

        emit cartChanged();
        emit checkoutSuccess("Order placed successfully!");
        return true;
    } else {
        qDebug() << "[CartController] ✗ Checkout failed:" << QString::fromStdString(result.error());
        emit checkoutFailed(QString::fromStdString(result.error()));
        return false;
    }
}

QVariantList CartController::getCartItems() const {
    QVariantList items;

    if (!m_buyer) {
        return items;
    }

    const auto& cartItems = m_buyer->getCart().getItems();

    for (const auto& [weakProduct, quantity] : cartItems) {
        auto product = weakProduct.lock();
        if (!product) {
            continue;
        }

        QVariantMap item;
        item["productId"] = QString::fromStdString(product->getID());
        item["name"] = QString::fromStdString(product->getName());
        item["price"] = product->getPrice();
        item["quantity"] = quantity;
        item["subtotal"] = product->getPrice() * quantity;
        item["stock"] = product->getStock();
        item["image"] = "📦";  // You can add image logic here

        items.append(item);
    }

    return items;
}
