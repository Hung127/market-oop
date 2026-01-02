#ifndef AUTHCONTROLLER_H
#define AUTHCONTROLLER_H

#include <QObject>
#include <QVariantMap>

class ApiService;
class StorageService;
class UserModel;

class AuthController : public QObject {
	Q_OBJECT
public:
	explicit AuthController(QObject *parent = nullptr);

	Q_INVOKABLE QVariantMap login(const QString &username, const QString &password);
	Q_INVOKABLE void logout();
	Q_INVOKABLE bool isLoggedIn() const;
	Q_INVOKABLE QString token() const;

signals:
	void loggedInChanged();

private:
	ApiService *m_api;
	StorageService *m_storage;
	UserModel *m_user;
	QString m_token;
};

#endif // AUTHCONTROLLER_H

