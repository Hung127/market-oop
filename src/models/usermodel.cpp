#include "usermodel.h"

UserModel::UserModel(QObject *parent)
	: QObject(parent)
{
}

void UserModel::setUser(const QVariantMap &data)
{
	m_username = data.value("username").toString();
	m_loggedIn = !m_username.isEmpty();
	emit userChanged();
}

void UserModel::clear()
{
	m_username.clear();
	m_loggedIn = false;
	emit userChanged();
}

QString UserModel::username() const
{
	return m_username;
}

bool UserModel::loggedIn() const
{
	return m_loggedIn;
}

