#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QObject>
#include <QString>
#include <memory>

#include "../DTO/UserDTO.h"

// Forward declaration
class ProductController;

class AuthController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString currentUserName READ currentUserName NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserEmail READ currentUserEmail NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserRole READ currentUserRole NOTIFY currentUserChanged)
    Q_PROPERTY(double currentBalance READ currentBalance NOTIFY currentUserChanged)
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY isLoggedInChanged)

    Q_PROPERTY(ProductController* productController READ productController CONSTANT)

   public:
    explicit AuthController(QObject* parent = nullptr);

    // Property getters
    QString currentUserName() const;
    QString currentUserEmail() const;
    QString currentUserRole() const;
    double currentBalance() const;
    bool isLoggedIn() const {
        return m_currentUser != nullptr;
    }

    // ✅ Getter for ProductController
    ProductController* productController() const {
        return m_productController;
    }

    // Methods callable from QML
    Q_INVOKABLE void login(const QString& email, const QString& password);
    Q_INVOKABLE void registerUser(const QString& name, const QString& email,
                                  const QString& password, const QString& role);
    Q_INVOKABLE void logout();

   signals:
    void loginSuccess();
    void loginFailed(const QString& error);
    void registerSuccess();
    void registerFailed(const QString& error);
    void isLoggedInChanged();
    void currentUserChanged();

   private:
    std::shared_ptr<UserDTO> m_currentUser;
    ProductController* m_productController;
};

#endif  // AUTHCONTROLLER_H
