#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <memory>

#include "include/BUS/UserBUS.h"
#include "include/DAO/BuyerDAO.h"
#include "include/DAO/OrderDAO.h"
#include "include/DAO/ProductDAO.h"
#include "include/DAO/SellerDAO.h"
#include "include/DAO/UserDAO.h"
#include "include/DTO/ProductDTO.h"
#include "include/DTO/SellerDTO.h"
#include "include/UserFactory.h"
#include "include/Utils/Utils.h"
#include "include/controllers/AuthController.h"
#include "include/controllers/ProductController.h"
#include "include/database/DatabaseManager.h"

// ============================================
// Helper function to create mock data
// ============================================
void createMockData() {
    qDebug() << "[main] Creating mock data... ";

    // Check if we already have products (avoid duplicates)
    auto existingProducts = ProductDAO::getAllProducts();
    if (!existingProducts.empty()) {
        qDebug() << "[main] Mock data already exists, skipping... ";
        return;
    }

    // ============================================
    // 1. Create Mock Sellers
    // ============================================
    struct MockSeller {
        std::string email;
        std::string name;
        std::string password;
    };

    std::vector<MockSeller> mockSellers = {
        {"techstore@example.com", "Tech Store", "password123"},
        {"gadgetworld@example.com", "Gadget World", "password123"},
        {"electronicshub@example.com", "Electronics Hub", "password123"}};

    std::vector<std::shared_ptr<SellerDTO>> sellers;

    for (const auto& mockSeller : mockSellers) {
        // Register seller
        auto result = UserBUS::registerUser(UserRole::SELLER, mockSeller.name, mockSeller.email,
                                            mockSeller.password,
                                            0.0  // Sellers don't have balance
        );

        if (result.has_value()) {
            auto seller = std::dynamic_pointer_cast<SellerDTO>(result.value());
            if (seller) {
                sellers.push_back(seller);
                qDebug() << "  ✓ Created seller:" << QString::fromStdString(mockSeller.name);
            }
        } else {
            qDebug() << "  ✗ Failed to create seller:" << QString::fromStdString(mockSeller.name)
                     << "-" << QString::fromStdString(result.error());
        }
    }

    // ============================================
    // 2. Create Mock Products
    // ============================================
    struct MockProduct {
        std::string name;
        std::string description;
        double price;
        int stock;
        int sellerIndex;  // Which seller owns this product
    };

    std::vector<MockProduct> mockProducts = {
        // Tech Store products
        {"Laptop Pro 15", "High-performance laptop with Intel i7, 16GB RAM, 512GB SSD", 1299.99, 10,
         0},
        {"Wireless Mouse", "Ergonomic wireless mouse with precision tracking", 29.99, 50, 0},
        {"Mechanical Keyboard", "RGB backlit mechanical keyboard with blue switches", 89.99, 30, 0},
        {"USB-C Hub", "7-in-1 USB-C hub with HDMI, USB 3.0, SD card reader", 49.99, 25, 0},

        // Gadget World products
        {"Smartphone X", "Latest smartphone with 5G, 128GB storage, triple camera", 899.99, 15, 1},
        {"Wireless Headphones", "Noise-cancelling over-ear headphones with 30hr battery", 199.99,
         40, 1},
        {"Smart Watch", "Fitness tracker with heart rate monitor and GPS", 249.99, 20, 1},
        {"Bluetooth Speaker", "Portable waterproof speaker with 360° sound", 79.99, 35, 1},

        // Electronics Hub products
        {"4K Monitor", "27-inch 4K UHD monitor with HDR and 144Hz refresh rate", 399.99, 12, 2},
        {"Webcam HD", "1080p webcam with built-in microphone and auto-focus", 69.99, 45, 2},
        {"External SSD 1TB", "Portable external SSD with USB 3.1 Gen 2, 1TB capacity", 129.99, 28,
         2},
        {"Gaming Mouse Pad", "Extended RGB gaming mouse pad with non-slip base", 24.99, 60, 2},

        // More Tech Store products
        {"Laptop Stand", "Adjustable aluminum laptop stand for better ergonomics", 34.99, 40, 0},
        {"Phone Charger", "Fast charging USB-C charger 20W with cable", 19.99, 80, 0},

        // More Gadget World products
        {"Tablet 10\"", "Android tablet with 10-inch display, 64GB storage", 299.99, 18, 1},
        {"Fitness Band", "Water-resistant fitness band with sleep tracking", 59.99, 50, 1},

        // More Electronics Hub products
        {"HDMI Cable 2m", "Premium HDMI 2.1 cable for 4K 120Hz gaming", 14.99, 100, 2},
        {"Power Bank", "20000mAh power bank with dual USB-C ports", 39.99, 45, 2}};

    // Create products for each seller
    int productCount = 0;
    for (const auto& mockProduct : mockProducts) {
        if (mockProduct.sellerIndex >= static_cast<int>(sellers.size())) {
            qDebug() << "  ✗ Invalid seller index for product:"
                     << QString::fromStdString(mockProduct.name);
            continue;
        }

        auto seller = sellers[mockProduct.sellerIndex];

        // Generate unique product ID
        std::string productId = "PROD-" + std::to_string(productCount + 1);

        // Create product
        auto product =
            std::make_shared<ProductDTO>(productId, mockProduct.name, mockProduct.price,
                                         mockProduct.stock, seller, mockProduct.description,
                                         ""  // No image path for mock data
            );

        // Save to database
        if (ProductDAO::insert(product)) {
            seller->addProduct(product);  // Add to seller's inventory
            productCount++;
            qDebug() << "  ✓ Created product:" << QString::fromStdString(mockProduct.name) << "("
                     << mockProduct.price << "," << mockProduct.stock << ")"
                     << "- Seller:" << QString::fromStdString(seller->getName());
        } else {
            qDebug() << "  ✗ Failed to create product:" << QString::fromStdString(mockProduct.name);
        }
    }

    // ============================================
    // 3. Create a Mock Buyer for testing
    // ============================================
    auto buyerResult =
        UserBUS::registerUser(UserRole::BUYER, "Test Buyer", "buyer@example.com", "password123",
                              5000.0  // Initial balance
        );

    if (buyerResult.has_value()) {
        qDebug() << "  ✓ Created mock buyer:  Test Buyer (Balance: $5000)";
    } else {
        qDebug() << "  ✗ Failed to create mock buyer:"
                 << QString::fromStdString(buyerResult.error());
    }

    qDebug() << "[main] ✓ Mock data created successfully! ";
    qDebug() << "       - Sellers:" << sellers.size();
    qDebug() << "       - Products:" << productCount;
    qDebug() << "";
    qDebug() << "Login credentials: ";
    qDebug() << "  Seller 1: techstore@example.com / password123";
    qDebug() << "  Seller 2: gadgetworld@example.com / password123";
    qDebug() << "  Seller 3: electronicshub@example.com / password123";
    qDebug() << "  Buyer: buyer@example.com / password123 (Balance: $5000)";
}

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
    // 2. Create Mock Data (only on first run)
    // ============================================
    createMockData();

    // ============================================
    // 3. Create Controllers
    // ============================================
    AuthController authController;
    ProductController productController;

    // ============================================
    // 4. Setup QML Engine
    // ============================================
    QQmlApplicationEngine engine;

    // Expose controllers to QML
    engine.rootContext()->setContextProperty("authController", &authController);
    engine.rootContext()->setContextProperty("productController", &productController);

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
