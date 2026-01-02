#include "storageservice.h"
#include <QSettings>

StorageService::StorageService(QObject *parent) : QObject(parent)
{
}

void StorageService::saveAuthToken(const QString &token)
{
	QSettings s;
	s.setValue("auth/token", token);
	s.sync();
}

QString StorageService::loadAuthToken() const
{
	QSettings s;
	return s.value("auth/token", QString()).toString();
}

void StorageService::clearAuthToken()
{
	QSettings s;
	s.remove("auth/token");
	s.sync();
}
