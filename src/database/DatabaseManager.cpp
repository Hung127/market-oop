#include "../../include/database/DatabaseManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>

DatabaseManager::DatabaseManager(const std::string& dbPath) : _db(nullptr), _dbPath(dbPath) {
    // Ensure data directory exists
    std::filesystem::path path(dbPath);
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }
}

DatabaseManager::~DatabaseManager() {
    if (_db) {
        sqlite3_close(_db);
        _db = nullptr;
    }
}

// Move constructor
DatabaseManager::DatabaseManager(DatabaseManager&& other) noexcept
    : _db(other._db), _dbPath(std::move(other._dbPath)) {
    other._db = nullptr;
}

// Move assignment
DatabaseManager& DatabaseManager::operator=(DatabaseManager&& other) noexcept {
    if (this != &other) {
        if (_db) {
            sqlite3_close(_db);
        }
        _db = other._db;
        _dbPath = std::move(other._dbPath);
        other._db = nullptr;
    }
    return *this;
}

bool DatabaseManager::initialize() {
    int rc = sqlite3_open(_dbPath.c_str(), &_db);
    if (rc != SQLITE_OK) {
        std::cerr << "[ERROR] Cannot open database: " << sqlite3_errmsg(_db) << std::endl;
        return false;
    }

    std::cout << "[INFO] Database opened successfully:  " << _dbPath << std::endl;

    // Enable foreign keys (IMPORTANT!)
    if (!executeSQL("PRAGMA foreign_keys = ON;")) {
        std::cerr << "[ERROR] Failed to enable foreign keys" << std::endl;
        return false;
    }

    // Set journal mode to WAL for better concurrency
    if (!executeSQL("PRAGMA journal_mode = WAL;")) {
        std::cerr << "[WARN] Failed to set WAL mode" << std::endl;
    }

    // Create tables
    if (!createTables()) {
        std::cerr << "[ERROR] Failed to create tables" << std::endl;
        return false;
    }

    std::cout << "[INFO] Database initialized successfully" << std::endl;
    return true;
}

bool DatabaseManager::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "[ERROR] SQL error: " << errMsg << std::endl;
        std::cerr << "[ERROR] SQL was: " << sql << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

std::expected<void, std::string> DatabaseManager::executeSQLWithError(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        return std::unexpected(error);
    }
    return {};
}

bool DatabaseManager::createTables() {
    // =============================================
    // 1. USERS TABLE (buyers and sellers)
    // =============================================
    std::string createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            type TEXT NOT NULL CHECK(type IN ('buyer', 'seller')),
            balance REAL DEFAULT 0.0,  -- ADD THIS LINE
            created_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
        
        CREATE INDEX IF NOT EXISTS idx_users_email ON users(email);
        CREATE INDEX IF NOT EXISTS idx_users_type ON users(type);
    )";

    // =============================================
    // 2. PRODUCTS TABLE
    // =============================================
    std::string createProductsTable = R"(
        CREATE TABLE IF NOT EXISTS products (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            price REAL NOT NULL CHECK(price >= 0),
            stock INTEGER NOT NULL CHECK(stock >= 0),
            seller_id TEXT NOT NULL,
            description TEXT,
            image_path TEXT,                      -- new column to store single image path
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (seller_id) REFERENCES users(id) ON DELETE CASCADE
        );
        
        CREATE INDEX IF NOT EXISTS idx_products_seller ON products(seller_id);
        CREATE INDEX IF NOT EXISTS idx_products_name ON products(name);
        CREATE INDEX IF NOT EXISTS idx_products_price ON products(price);
    )";

    // =============================================
    // 3. MARKET LISTINGS TABLE
    // =============================================
    // Tracks which products are actively listed on the market
    std::string createMarketListingsTable = R"(
        CREATE TABLE IF NOT EXISTS market_listings (
            product_id TEXT PRIMARY KEY,
            listed_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE
        );
        
        CREATE INDEX IF NOT EXISTS idx_market_listed_at ON market_listings(listed_at);
    )";

    // =============================================
    // 4. CART ITEMS TABLE
    // =============================================
    // Persistent cart for buyers
    std::string createCartsTable = R"(
        CREATE TABLE IF NOT EXISTS cart_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            buyer_id TEXT NOT NULL,
            product_id TEXT NOT NULL,
            quantity INTEGER NOT NULL CHECK(quantity > 0),
            added_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (buyer_id) REFERENCES users(id) ON DELETE CASCADE,
            FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE CASCADE,
            UNIQUE(buyer_id, product_id)
        );
        
        CREATE INDEX IF NOT EXISTS idx_cart_buyer ON cart_items(buyer_id);
    )";

    // =============================================
    // 5. ORDERS TABLE
    // =============================================
    std::string createOrdersTable = R"(
        CREATE TABLE IF NOT EXISTS orders (
            id TEXT PRIMARY KEY,
            buyer_id TEXT NOT NULL,
            total_amount REAL NOT NULL CHECK(total_amount >= 0),
            status TEXT NOT NULL DEFAULT 'pending' 
                CHECK(status IN ('pending', 'processing', 'completed', 'cancelled')),
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (buyer_id) REFERENCES users(id) ON DELETE CASCADE
        );
        
        CREATE INDEX IF NOT EXISTS idx_orders_buyer ON orders(buyer_id);
        CREATE INDEX IF NOT EXISTS idx_orders_status ON orders(status);
        CREATE INDEX IF NOT EXISTS idx_orders_created ON orders(created_at);
    )";

    // =============================================
    // 6. ORDER ITEMS TABLE
    // =============================================
    std::string createOrderItemsTable = R"(
        CREATE TABLE IF NOT EXISTS order_items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            order_id TEXT NOT NULL,
            product_id TEXT NOT NULL,
            product_name TEXT NOT NULL,
            seller_id TEXT NOT NULL,
            quantity INTEGER NOT NULL CHECK(quantity > 0),
            price REAL NOT NULL CHECK(price >= 0),
            subtotal REAL NOT NULL CHECK(subtotal >= 0),
            status TEXT NOT NULL DEFAULT 'pending'
                CHECK(status IN ('pending', 'confirmed', 'shipped', 'delivered', 'cancelled')),
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (order_id) REFERENCES orders(id) ON DELETE CASCADE,
            FOREIGN KEY (seller_id) REFERENCES users(id)
        );
        
        CREATE INDEX IF NOT EXISTS idx_order_items_order ON order_items(order_id);
        CREATE INDEX IF NOT EXISTS idx_order_items_seller ON order_items(seller_id);
        CREATE INDEX IF NOT EXISTS idx_order_items_status ON order_items(status);
        CREATE INDEX IF NOT EXISTS idx_order_items_product ON order_items(product_id);
    )";

    // =============================================
    // 7. PURCHASE HISTORY VIEW (Optional)
    // =============================================
    // A view to easily query purchase history
    std::string createPurchaseHistoryView = R"(
        CREATE VIEW IF NOT EXISTS purchase_history AS
        SELECT 
            o.id as order_id,
            o. buyer_id,
            u.name as buyer_name,
            o.total_amount,
            o.status as order_status,
            o. created_at as order_date,
            oi.product_id,
            oi.product_name,
            oi. quantity,
            oi.price,
            oi.subtotal,
            oi.seller_id,
            s.name as seller_name,
            oi.status as item_status
        FROM orders o
        JOIN users u ON o.buyer_id = u.id
        JOIN order_items oi ON o.id = oi.order_id
        JOIN users s ON oi.seller_id = s.id;
    )";

    // =============================================
    // 8. SELLER SALES VIEW (Optional)
    // =============================================
    std::string createSellerSalesView = R"(
        CREATE VIEW IF NOT EXISTS seller_sales AS
        SELECT 
            oi.seller_id,
            u.name as seller_name,
            COUNT(DISTINCT oi.order_id) as total_orders,
            COUNT(oi.id) as total_items_sold,
            SUM(oi.quantity) as total_quantity_sold,
            SUM(oi.subtotal) as total_revenue,
            oi.status as item_status
        FROM order_items oi
        JOIN users u ON oi. seller_id = u.id
        GROUP BY oi.seller_id, oi.status;
    )";

    // Execute all table creations
    std::cout << "[INFO] Creating database tables..." << std::endl;

    if (!executeSQL(createUsersTable)) {
        std::cerr << "[ERROR] Failed to create users table" << std::endl;
        return false;
    }
    std::cout << "[INFO] ✓ Users table created" << std::endl;

    if (!executeSQL(createProductsTable)) {
        std::cerr << "[ERROR] Failed to create products table" << std::endl;
        return false;
    }
    std::cout << "[INFO] ✓ Products table created" << std::endl;

    if (!executeSQL(createMarketListingsTable)) {
        std::cerr << "[ERROR] Failed to create market_listings table" << std::endl;
        return false;
    }
    std::cout << "[INFO] ✓ Market listings table created" << std::endl;

    if (!executeSQL(createCartsTable)) {
        std::cerr << "[ERROR] Failed to create cart_items table" << std::endl;
        return false;
    }
    std::cout << "[INFO] ✓ Cart items table created" << std::endl;

    if (!executeSQL(createOrdersTable)) {
        std::cerr << "[ERROR] Failed to create orders table" << std::endl;
        return false;
    }
    std::cout << "[INFO] ✓ Orders table created" << std::endl;

    if (!executeSQL(createOrderItemsTable)) {
        std::cerr << "[ERROR] Failed to create order_items table" << std::endl;
        return false;
    }
    std::cout << "[INFO] ✓ Order items table created" << std::endl;

    // Create views (non-critical, so just warn on failure)
    if (!executeSQL(createPurchaseHistoryView)) {
        std::cerr << "[WARN] Failed to create purchase_history view" << std::endl;
    } else {
        std::cout << "[INFO] ✓ Purchase history view created" << std::endl;
    }

    if (!executeSQL(createSellerSalesView)) {
        std::cerr << "[WARN] Failed to create seller_sales view" << std::endl;
    } else {
        std::cout << "[INFO] ✓ Seller sales view created" << std::endl;
    }

    std::cout << "[INFO] All tables created successfully" << std::endl;
    return true;
}

bool DatabaseManager::dropAllTables() {
    std::cout << "[WARN] Dropping all tables..." << std::endl;

    // Drop views first
    executeSQL("DROP VIEW IF EXISTS seller_sales;");
    executeSQL("DROP VIEW IF EXISTS purchase_history;");

    // Drop tables in reverse order of dependencies
    return executeSQL("DROP TABLE IF EXISTS order_items;") &&
           executeSQL("DROP TABLE IF EXISTS orders;") &&
           executeSQL("DROP TABLE IF EXISTS cart_items;") &&
           executeSQL("DROP TABLE IF EXISTS market_listings;") &&
           executeSQL("DROP TABLE IF EXISTS products;") &&
           executeSQL("DROP TABLE IF EXISTS users;");
}

// =============================================
// TRANSACTION MANAGEMENT
// =============================================

bool DatabaseManager::beginTransaction() {
    return executeSQL("BEGIN TRANSACTION;");
}

bool DatabaseManager::commitTransaction() {
    return executeSQL("COMMIT;");
}

bool DatabaseManager::rollbackTransaction() {
    return executeSQL("ROLLBACK;");
}

// =============================================
// UTILITY METHODS
// =============================================

bool DatabaseManager::tableExists(const std::string& tableName) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?;";

    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);

    return exists;
}

int DatabaseManager::getLastInsertRowId() {
    return static_cast<int>(sqlite3_last_insert_rowid(_db));
}

std::string DatabaseManager::getLastError() {
    return sqlite3_errmsg(_db);
}

int DatabaseManager::executeCount(const std::string& sql) {
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

// =============================================
// BACKUP AND RESTORE
// =============================================

bool DatabaseManager::backupDatabase(const std::string& backupPath) {
    sqlite3* backupDb;

    int rc = sqlite3_open(backupPath.c_str(), &backupDb);
    if (rc != SQLITE_OK) {
        std::cerr << "[ERROR] Cannot open backup database: " << sqlite3_errmsg(backupDb)
                  << std::endl;
        return false;
    }

    sqlite3_backup* backup = sqlite3_backup_init(backupDb, "main", _db, "main");
    if (!backup) {
        std::cerr << "[ERROR] Failed to initialize backup" << std::endl;
        sqlite3_close(backupDb);
        return false;
    }

    // Perform the backup
    rc = sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);

    if (rc != SQLITE_DONE) {
        std::cerr << "[ERROR] Backup failed: " << sqlite3_errmsg(backupDb) << std::endl;
        sqlite3_close(backupDb);
        return false;
    }

    sqlite3_close(backupDb);
    std::cout << "[INFO] Database backed up to: " << backupPath << std::endl;
    return true;
}

bool DatabaseManager::restoreDatabase(const std::string& backupPath) {
    sqlite3* backupDb;

    int rc = sqlite3_open(backupPath.c_str(), &backupDb);
    if (rc != SQLITE_OK) {
        std::cerr << "[ERROR] Cannot open backup database: " << sqlite3_errmsg(backupDb)
                  << std::endl;
        return false;
    }

    sqlite3_backup* backup = sqlite3_backup_init(_db, "main", backupDb, "main");
    if (!backup) {
        std::cerr << "[ERROR] Failed to initialize restore" << std::endl;
        sqlite3_close(backupDb);
        return false;
    }

    // Perform the restore
    rc = sqlite3_backup_step(backup, -1);
    sqlite3_backup_finish(backup);

    if (rc != SQLITE_DONE) {
        std::cerr << "[ERROR] Restore failed: " << sqlite3_errmsg(_db) << std::endl;
        sqlite3_close(backupDb);
        return false;
    }

    sqlite3_close(backupDb);
    std::cout << "[INFO] Database restored from: " << backupPath << std::endl;
    return true;
}

// =============================================
// DATABASE MAINTENANCE
// =============================================

bool DatabaseManager::vacuum() {
    std::cout << "[INFO] Running VACUUM to optimize database..." << std::endl;
    return executeSQL("VACUUM;");
}

bool DatabaseManager::analyze() {
    std::cout << "[INFO] Running ANALYZE to update statistics..." << std::endl;
    return executeSQL("ANALYZE;");
}
