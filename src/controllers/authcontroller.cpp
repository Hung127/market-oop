#include "authcontroller.h"
#include "../services/apiservice.h"
#include "../services/storageservice.h"
#include "../models/usermodel.h"

AuthController::AuthController(QObject *parent)
	: QObject(parent)
{
	m_api = new ApiService(this);
	m_storage = new StorageService(this);
	m_user = new UserModel(this);
	m_token = m_storage->loadAuthToken();
}

QVariantMap AuthController::login(const QString &username, const QString &password)
{
	QVariantMap resp = m_api->login(username, password);
	if (resp.value("success").toBool()) {
		m_token = resp.value("token").toString();
		m_storage->saveAuthToken(m_token);
		QVariantMap user = resp.value("user").toMap();
		m_user->setUser(user);
		emit loggedInChanged();
	}
	return resp;
}

void AuthController::logout()
{
	m_token.clear();
	m_storage->clearAuthToken();
	m_user->clear();
	emit loggedInChanged();
}

bool AuthController::isLoggedIn() const
{
	return !m_token.isEmpty();
}

QString AuthController::token() const
{
	return m_token;
}

