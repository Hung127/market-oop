#include "../../include/controllers/AuthController.h"

#include <QDebug>

#include "../../include/BUS/UserBUS.h"
#include "../../include/DTO/BuyerDTO.h"
#include "../../include/DTO/SellerDTO.h"
#include "../../include/DTO/UserDTO.h"
#include "../../include/UserFactory.h"
#include "../../include/controllers/CartController.h"
#include "../../include/models/ProductModel.h"

AuthController::AuthController(QObject* parent)
    : QObject(parent),
      m_currentUser(nullptr),
      m_productModel(new ProductModel(this)),
      m_cartController(new CartController(this)) {  // Add this
    qDebug() << "[AuthController] Initialized";

    // Load products on startup
    m_productModel->loadProducts();
}

QString AuthController::currentUserName() const {
    return m_currentUser ? QString::fromStdString(m_currentUser->getName()) : "";
}

QString AuthController::currentUserEmail() const {
    return m_currentUser ? QString::fromStdString(m_currentUser->getEmail()) : "";
}

QString AuthController::currentUserRole() const {
    return m_currentUser ? QString::fromStdString(m_currentUser->getRole()) : "";
}

double AuthController::currentBalance() const {
    if (m_currentUser) {
        auto buyer = std::dynamic_pointer_cast<BuyerDTO>(m_currentUser);
        if (buyer) {
            return buyer->getBalance();
        }
    }
    return 0.0;
}

void AuthController::login(const QString& email, const QString& password) {
    qDebug() << "[AuthController] Login attempt for:" << email;

    auto result = UserBUS::login(email.toStdString(), password.toStdString());

    if (result.has_value()) {
        m_currentUser = result.value();
        qDebug() << "[AuthController] ✓ Login successful";
        qDebug() << "  User:" << QString::fromStdString(m_currentUser->getName());
        qDebug() << "  Role:" << QString::fromStdString(m_currentUser->getRole());

        // If buyer, set up cart controller
        if (auto buyer = std::dynamic_pointer_cast<BuyerDTO>(m_currentUser)) {
            m_cartController->setBuyer(buyer);
            qDebug() << "[AuthController] Cart controller initialized for buyer";
        }

        emit currentUserChanged();
        emit isLoggedInChanged();
        emit loginSuccess();
    } else {
        qDebug() << "[AuthController] ✗ Login failed:" << QString::fromStdString(result.error());
        emit loginFailed(QString::fromStdString(result.error()));
    }
}

void AuthController::registerUser(const QString& name, const QString& email,
                                  const QString& password, const QString& role) {
    qDebug() << "[AuthController] Register attempt:  ";
    qDebug() << "  Name:" << name;
    qDebug() << "  Email:" << email;
    qDebug() << "  Role:" << role;

    UserRole userRole = (role.toLower() == "buyer") ? UserRole::BUYER : UserRole::SELLER;

    auto result = UserBUS::registerUser(userRole, name.toStdString(), email.toStdString(),
                                        password.toStdString(), 1000.0);

    if (result.has_value()) {
        qDebug() << "[AuthController] ✓ Registration successful";
        emit registerSuccess();
    } else {
        qDebug() << "[AuthController] ✗ Registration failed:"
                 << QString::fromStdString(result.error());
        emit registerFailed(QString::fromStdString(result.error()));
    }
}

void AuthController::logout() {
    qDebug() << "[AuthController] Logging out";
    m_currentUser.reset();
    m_cartController->setBuyer(nullptr);  // Clear cart
    emit isLoggedInChanged();
    emit currentUserChanged();
}
