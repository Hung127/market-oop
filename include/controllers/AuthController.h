#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QObject>
#include <QString>
#include <memory>

class UserDTO;
class ProductModel;
class CartController;  // Add this

class AuthController : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool isLoggedIn READ isLoggedIn NOTIFY isLoggedInChanged)
    Q_PROPERTY(QString currentUserName READ currentUserName NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserEmail READ currentUserEmail NOTIFY currentUserChanged)
    Q_PROPERTY(QString currentUserRole READ currentUserRole NOTIFY currentUserChanged)
    Q_PROPERTY(double currentBalance READ currentBalance NOTIFY currentUserChanged)
    Q_PROPERTY(ProductModel* productModel READ productModel CONSTANT)
    Q_PROPERTY(CartController* cartController READ cartController CONSTANT)  // Add this

   public:
    explicit AuthController(QObject* parent = nullptr);
    ~AuthController() override = default;

    // Property getters
    bool isLoggedIn() const {
        return m_currentUser != nullptr;
    }
    QString currentUserName() const;
    QString currentUserEmail() const;
    QString currentUserRole() const;
    double currentBalance() const;

    ProductModel* productModel() const {
        return m_productModel;
    }

    CartController* cartController() const {  // Add this
        return m_cartController;
    }

    // Invokable methods
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
    ProductModel* m_productModel;
    CartController* m_cartController;  // Add this
};

#endif  // AUTHCONTROLLER_H
