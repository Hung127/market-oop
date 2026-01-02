#ifndef APISERVICE_H
#define APISERVICE_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class ApiService : public QObject {
	Q_OBJECT
public:
	explicit ApiService(QObject *parent = nullptr);

	// Trả về URL gốc của API
	Q_INVOKABLE QString baseUrl() const;

	// Giả lập gọi API trả về dữ liệu mặc định (synchronous stub)
	Q_INVOKABLE QVariantMap fetch(const QString &endpoint, const QVariantMap &params = QVariantMap());

	// Giả lập đăng nhập, trả về token giả
	Q_INVOKABLE QVariantMap login(const QString &username, const QString &password);
};

#endif // APISERVICE_H
