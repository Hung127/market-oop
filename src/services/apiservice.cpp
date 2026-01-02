#include "apiservice.h"
#include <QVariantList>

ApiService::ApiService(QObject *parent) : QObject(parent)
{
}

QString ApiService::baseUrl() const
{
	return QStringLiteral("https://api.example.com/");
}

QVariantMap ApiService::fetch(const QString &endpoint, const QVariantMap &params)
{
	Q_UNUSED(params)
	QVariantMap result;
	// Đây là stub: trả về dữ liệu giả để UI có thể chạy mà không cần backend.
	result.insert("endpoint", endpoint);
	result.insert("status", "ok");
	result.insert("data", QVariantList{});
	return result;
}

QVariantMap ApiService::login(const QString &username, const QString &password)
{
	Q_UNUSED(password)
	QVariantMap r;
	if (username.isEmpty()) {
		r.insert("success", false);
		r.insert("error", "empty_username");
		return r;
	}

	// Trả token giả để phần còn lại của app có thể dùng
	r.insert("success", true);
	r.insert("token", QString("fake-token-%1").arg(username));
	r.insert("user", QVariantMap{{"username", username}});
	return r;
}
