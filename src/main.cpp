#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "productmodel.h"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    // Flash Sale Products
    ProductModel flashSaleModel;
    flashSaleModel.addProduct({"HAVIT HV-G92 Gamepad", 120, 160, 40, 4.9, 88,
                               "qrc:/images/gamepad.png"});
    flashSaleModel.addProduct({"AK-900 Wired Keyboard", 960, 1160, 35, 4.5, 75,
                               "qrc:/images/keyboard.png"});
    flashSaleModel.addProduct({"IPS LCD Gaming Monitor", 370, 400, 30, 5.0, 99,
                               "qrc:/images/monitor.png"});
    flashSaleModel.addProduct({"S-Series Comfort Chair", 375, 400, 25, 4.7, 99,
                               "qrc:/images/chair.png"});
    flashSaleModel.addProduct({"RGB Liquid CPU Cooler", 160, 170, 20, 4.8, 65,
                               "qrc:/images/cooler.png"});

    // Best Selling Products
    ProductModel bestSellingModel;
    bestSellingModel.addProduct(
        {"The north coat", 260, 360, 28, 5.0, 65, "qrc:/images/coat.png"});
    bestSellingModel.addProduct(
        {"Gucci duffle bag", 960, 1160, 35, 4.5, 65, "qrc:/images/bag.png"});
    bestSellingModel.addProduct({"RGB liquid CPU Cooler", 160, 170, 20, 4.8, 65,
                                 "qrc:/images/cooler.png"});
    bestSellingModel.addProduct(
        {"Small BookSelf", 360, 0, 0, 5.0, 65, "qrc:/images/bookshelf.png"});

    // All Products (Explore Products)
    ProductModel allProductsModel;
    allProductsModel.addProduct(
        {"Breed Dry Dog Food", 100, 0, 0, 3.5, 35, "qrc:/images/dogfood.png"});
    allProductsModel.addProduct({"CANON EOS DSLR Camera", 360, 0, 0, 4.0, 95,
                                 "qrc:/images/camera.png"});
    allProductsModel.addProduct({"ASUS FHD Gaming Laptop", 700, 0, 0, 5.0, 325,
                                 "qrc:/images/laptop.png"});
    allProductsModel.addProduct({"Curology Product Set", 500, 0, 0, 4.0, 145,
                                 "qrc:/images/curology.png"});
    allProductsModel.addProduct(
        {"Kids Electric Car", 960, 0, 0, 5.0, 65, "qrc:/images/car.png"});
    allProductsModel.addProduct({"Jr. Zoom Soccer Cleats", 1160, 0, 0, 4.5, 35,
                                 "qrc:/images/cleats.png"});
    allProductsModel.addProduct({"GP11 Shooter USB Gamepad", 660, 0, 0, 4.5, 55,
                                 "qrc:/images/gamepad2.png"});
    allProductsModel.addProduct(
        {"Quilted Satin Jacket", 660, 0, 0, 4.5, 55, "qrc:/images/jacket.png"});

    QQmlApplicationEngine engine;

    // Set context properties before loading QML
    engine.rootContext()->setContextProperty("flashSaleModel", &flashSaleModel);
    engine.rootContext()->setContextProperty("bestSellingModel",
                                             &bestSellingModel);
    engine.rootContext()->setContextProperty("allProductsModel",
                                             &allProductsModel);

    // Đường dẫn QML với RESOURCE_PREFIX /
    const QUrl url(QStringLiteral("qrc:/ExclusiveEcommerce/Main.qml"));

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) {
                qDebug() << "Failed to load QML from:" << url;
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    engine.load(url);

    if (engine.rootObjects().isEmpty()) {
        qDebug() << "No root objects loaded!";
        return -1;
    }

    return app.exec();
}
