#ifndef STORAGESERVICE_H
#define STORAGESERVICE_H

#include <QObject>
#include <QString>

class StorageService : public QObject {
	Q_OBJECT
public:
	explicit StorageService(QObject *parent = nullptr);

	// Lưu/đọc token xác thực
	Q_INVOKABLE void saveAuthToken(const QString &token);
	Q_INVOKABLE QString loadAuthToken() const;
	Q_INVOKABLE void clearAuthToken();
};

#endif // STORAGESERVICE_H
