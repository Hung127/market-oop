#include "TestDbListener.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "../include/DAO/BuyerDAO.h"
#include "../include/DAO/OrderDAO.h"
#include "../include/DAO/ProductDAO.h"
#include "../include/DAO/SellerDAO.h"
#include "../include/DAO/UserDAO.h"
#include "../include/database/DatabaseManager.h"

static std::string sanitizeName(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-') {
            out.push_back(c);
        } else {
            out.push_back('_');
        }
    }
    return out;
}

void TestDbListener::OnTestStart(const ::testing::TestInfo& test_info) {
    // Construct a deterministic but unique filename for this test.
    // Using test suite + test name + timestamp to avoid collisions across parallel runs.
    std::ostringstream oss;
    oss << test_info.test_suite_name() << "_" << test_info.name();

    std::string key = sanitizeName(oss.str());

    // Append a timestamp (milliseconds) to be extra-safe
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    std::ostringstream dbname;
    dbname << "test_db_" << key << "_" << ms << ".db";

    std::string dbPath = dbname.str();

    // Remove any existing file (shouldn't be any, but be defensive)
    if (std::filesystem::exists(dbPath)) {
        std::error_code ec;
        std::filesystem::remove(dbPath, ec);
        if (ec) {
            std::cerr << "[WARN] TestDbListener: could not remove existing DB " << dbPath << " : "
                      << ec.message() << std::endl;
        }
    }

    auto dbManager = std::make_shared<DatabaseManager>(dbPath);
    if (!dbManager->initialize()) {
        std::cerr << "[ERROR] TestDbListener: failed to initialize DB at " << dbPath << std::endl;
        std::abort();
    }

    // Wire into DAOs used by tests
    ProductDAO::setDatabaseManager(dbManager);
    UserDAO::setDatabaseManager(dbManager);
    OrderDAO::setDatabaseManager(dbManager);
    BuyerDAO::setDatabaseManager(dbManager);
    SellerDAO::setDatabaseManager(dbManager);

    _active.emplace(key, std::make_pair(dbManager, dbPath));

    std::cout << "[INFO] TestDbListener: created test DB " << dbPath << " for test " << key
              << std::endl;
}

void TestDbListener::OnTestEnd(const ::testing::TestInfo& test_info) {
    std::ostringstream oss;
    oss << test_info.test_suite_name() << "_" << test_info.name();
    std::string key = sanitizeName(oss.str());

    auto it = _active.find(key);
    if (it == _active.end()) {
        // Nothing to tear down (shouldn't happen)
        return;
    }

    // Reset DAO database managers to null (disconnect)
    ProductDAO::setDatabaseManager(nullptr);
    UserDAO::setDatabaseManager(nullptr);
    OrderDAO::setDatabaseManager(nullptr);
    BuyerDAO::setDatabaseManager(nullptr);
    SellerDAO::setDatabaseManager(nullptr);

    // Destroy manager (shared_ptr reset)
    it->second.first.reset();

    // Remove DB file
    std::error_code ec;
    std::filesystem::remove(it->second.second, ec);
    if (ec) {
        std::cerr << "[WARN] TestDbListener: failed to remove " << it->second.second << ": "
                  << ec.message() << std::endl;
    } else {
        std::cout << "[INFO] TestDbListener: removed test DB " << it->second.second << std::endl;
    }

    _active.erase(it);
}
