#ifndef USERMODEL_H
#define USERMODEL_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class UserModel : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString username READ username NOTIFY userChanged)
	Q_PROPERTY(bool loggedIn READ loggedIn NOTIFY userChanged)
public:
	explicit UserModel(QObject *parent = nullptr);

	Q_INVOKABLE void setUser(const QVariantMap &data);
	Q_INVOKABLE void clear();

	QString username() const;
	bool loggedIn() const;

signals:
	void userChanged();

private:
	QString m_username;
	bool m_loggedIn{false};
};

#endif // USERMODEL_H

