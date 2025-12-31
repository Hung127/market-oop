#include <filesystem>
#include <iostream>
#include <memory>

#include <gtest/gtest.h>

#include "../include/DAO/BuyerDAO.h"
#include "../include/DAO/OrderDAO.h"
#include "../include/DAO/ProductDAO.h"
#include "../include/DAO/SellerDAO.h"
#include "../include/DAO/UserDAO.h"
#include "../include/database/DatabaseManager.h"

/*
  GlobalDbEnvironment:
  - Creates a single SQLite test DB ("test_market_global.db") before any tests run.
  - Wires the DatabaseManager into ProductDAO, UserDAO, OrderDAO, BuyerDAO, SellerDAO.
  - Tears down/ removes the DB file after all tests complete.
*/
class GlobalDbEnvironment : public ::testing::Environment {
   public:
    GlobalDbEnvironment() = default;
    ~GlobalDbEnvironment() override = default;

    void SetUp() override {
        testDbPath = "test_market_global.db";

        // remove any previous database to start clean
        if (std::filesystem::exists(testDbPath)) {
            std::filesystem::remove(testDbPath);
        }

        dbManager = std::make_shared<DatabaseManager>(testDbPath);
        if (!dbManager->initialize()) {
            std::cerr << "[ERROR] GlobalDbEnvironment: failed to initialize database at "
                      << testDbPath << std::endl;
            std::abort();
        }

        // Wire DB manager into DAOs used throughout tests
        ProductDAO::setDatabaseManager(dbManager);
        UserDAO::setDatabaseManager(dbManager);
        OrderDAO::setDatabaseManager(dbManager);
        BuyerDAO::setDatabaseManager(dbManager);
        SellerDAO::setDatabaseManager(dbManager);

        std::cout << "[INFO] GlobalDbEnvironment: test DB initialized at " << testDbPath
                  << std::endl;
    }

    void TearDown() override {
        // Destroy DB manager and delete test file
        dbManager.reset();

        if (std::filesystem::exists(testDbPath)) {
            std::error_code ec;
            std::filesystem::remove(testDbPath, ec);
            if (ec) {
                std::cerr << "[WARN] GlobalDbEnvironment: failed to remove " << testDbPath << ": "
                          << ec.message() << std::endl;
            } else {
                std::cout << "[INFO] GlobalDbEnvironment: removed test DB " << testDbPath
                          << std::endl;
            }
        }
    }

    std::shared_ptr<DatabaseManager> getDbManager() const {
        return dbManager;
    }

   private:
    std::shared_ptr<DatabaseManager> dbManager;
    std::string testDbPath;
};
