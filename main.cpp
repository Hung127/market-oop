#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <memory>

#include "include/DAO/BuyerDAO.h"
#include "include/DAO/OrderDAO.h"
#include "include/DAO/ProductDAO.h"
#include "include/DAO/SellerDAO.h"
#include "include/DAO/UserDAO.h"
#include "include/controllers/AuthController.h"
#include "include/database/DatabaseManager.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    // ============================================
    // 1. Initialize Database
    // ============================================
    auto dbManager = std::make_shared<DatabaseManager>("data/market.db");
    if (!dbManager->initialize()) {
        qFatal("Failed to initialize database");
        return -1;
    }

    // Wire DAOs
    UserDAO::setDatabaseManager(dbManager);
    BuyerDAO::setDatabaseManager(dbManager);
    SellerDAO::setDatabaseManager(dbManager);
    ProductDAO::setDatabaseManager(dbManager);
    OrderDAO::setDatabaseManager(dbManager);

    qDebug() << "[main] ✓ Database initialized";

    // ============================================
    // 2. Create Controllers
    // ============================================
    AuthController authController;

    // ============================================
    // 3. Setup QML Engine
    // ============================================
    QQmlApplicationEngine engine;

    // Expose controllers to QML
    engine.rootContext()->setContextProperty("authController", &authController);

    // Load main QML
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);

    engine.load(url);

    qDebug() << "[main] ✓ QML loaded";

    return app.exec();
}
