#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <expected>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include <sqlite3.h>

class DatabaseManager {
   private:
    sqlite3* _db;
    std::string _dbPath;
    
    // Singleton Pattern: Ensures only one instance of DatabaseManager exists globally.
    // This prevents multiple database connections and ensures thread-safety for DB operations.
    static std::unique_ptr<DatabaseManager> _instance;  // Singleton instance holder
    static std::mutex _mutex;  // For thread-safety in multi-threaded environments

    
    // Internal helper to execute SQL without parameters
    bool executeSQL(const std::string& sql);

    // Internal helper to execute SQL with error message
    std::expected<void, std::string> executeSQLWithError(const std::string& sql);

   public:
    // Singleton Pattern: Get the single instance of DatabaseManager.
    // Call this instead of new DatabaseManager() to ensure uniqueness.
    static DatabaseManager& getInstance(const std::string& dbPath = "data/market.db");

    DatabaseManager(const std::string& dbPath = "data/market.db");
    ~DatabaseManager();

    // Disable copy
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Move constructor and assignment
    DatabaseManager(DatabaseManager&& other) noexcept;
    DatabaseManager& operator=(DatabaseManager&& other) noexcept;

    // Initialization
    bool initialize();
    bool createTables();
    bool dropAllTables();  // For testing/reset

    // Connection getter
    sqlite3* getConnection() {
        return _db;
    }

    // Transaction management
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    // Utility methods
    bool tableExists(const std::string& tableName);
    int getLastInsertRowId();
    std::string getLastError();

    // Backup and restore
    bool backupDatabase(const std::string& backupPath);
    bool restoreDatabase(const std::string& backupPath);

    // Database maintenance
    bool vacuum();   // Optimize database
    bool analyze();  // Update query optimizer statistics

    // Query helpers
    int executeCount(const std::string& sql);  // For COUNT(*) queries
};

#endif
