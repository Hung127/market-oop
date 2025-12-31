#include "../../include/DAO/UserDAO.h"

#include <iostream>

#include <sqlite3.h>

#include "../../include/DTO/BuyerDTO.h"
#include "../../include/DTO/SellerDTO.h"
#include "../../include/DTO/UserDTO.h"
#include "../../include/database/DatabaseManager.h"

std::shared_ptr<DatabaseManager> UserDAO::_dbManager = nullptr;

void UserDAO::setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager) {
    _dbManager = dbManager;
}

// Helper function to create appropriate DTO based on type
std::shared_ptr<UserDTO> UserDAO::createUserFromRow(const std::string& id, const std::string& name,
                                                    const std::string& email,
                                                    const std::string& password,
                                                    const std::string& type, double balance) {
    if (type == "buyer") {
        return std::make_shared<BuyerDTO>(id, name, email, password, balance);
    } else if (type == "seller") {
        return std::make_shared<SellerDTO>(id, name, email, password);
    }
    return nullptr;
}

std::expected<std::shared_ptr<UserDTO>, std::string>
UserDAO::getUserByEmail(const std::string& email) {
    if (!_dbManager) {
        return std::unexpected("Database not initialized");
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, email, password, type, balance FROM users WHERE email = ? ";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string emailDb = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        double balance = sqlite3_column_double(stmt, 5);

        auto user = createUserFromRow(id, name, emailDb, password, type, balance);
        sqlite3_finalize(stmt);

        if (user) {
            return user;
        }
        return std::unexpected("Failed to create user object");
    }

    sqlite3_finalize(stmt);
    return std::unexpected("User not found with email: " + email);
}

std::expected<std::shared_ptr<UserDTO>, std::string> UserDAO::getUserById(const std::string& id) {
    if (!_dbManager) {
        return std::unexpected("Database not initialized");
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, email, password, type, balance FROM users WHERE id = ? ";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string idDb = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        double balance = sqlite3_column_double(stmt, 5);

        auto user = createUserFromRow(idDb, name, email, password, type, balance);
        sqlite3_finalize(stmt);

        if (user) {
            return user;
        }
        return std::unexpected("Failed to create user object");
    }

    sqlite3_finalize(stmt);
    return std::unexpected("User not found with ID: " + id);
}

std::expected<void, std::string> UserDAO::addUser(const std::shared_ptr<UserDTO>& user) {
    if (!_dbManager || !user) {
        return std::unexpected("Invalid parameters");
    }

    // Check if user already exists
    if (existById(user->getId())) {
        return std::unexpected("User with this ID already exists");
    }
    if (existByEmail(user->getEmail())) {
        return std::unexpected("User with this email already exists");
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "INSERT INTO users (id, name, email, password, type, balance) VALUES (?, ?, ?, ?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare insert statement");
    }

    // Determine user type
    std::string userType = user->getRole();  // "BUYER" or "SELLER"
    // Convert to lowercase for database
    for (char& c : userType) {
        c = std::tolower(c);
    }

    // Get balance (only for buyers, sellers have 0)
    double balance = 0.0;
    if (auto buyer = std::dynamic_pointer_cast<BuyerDTO>(user)) {
        balance = buyer->getBalance();
    }

    sqlite3_bind_text(stmt, 1, user->getId().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user->getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user->getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, user->getHashedPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, userType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 6, balance);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    if (!success) {
        return std::unexpected("Failed to insert user");
    }

    return {};
}

std::expected<void, std::string> UserDAO::updateUser(const std::shared_ptr<UserDTO>& user) {
    if (!_dbManager || !user) {
        return std::unexpected("Invalid parameters");
    }

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql =
        "UPDATE users SET name = ?, email = ?, password = ?, balance = ? WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::unexpected("Failed to prepare update statement");
    }

    // Get balance (only for buyers, sellers have 0)
    double balance = 0.0;
    if (auto buyer = std::dynamic_pointer_cast<BuyerDTO>(user)) {
        balance = buyer->getBalance();
    }

    sqlite3_bind_text(stmt, 1, user->getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, user->getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, user->getHashedPassword().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, balance);
    sqlite3_bind_text(stmt, 5, user->getId().c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);

    if (!success) {
        return std::unexpected("Failed to update user");
    }

    return {};
}

bool UserDAO::existById(const std::string& id) {
    if (!_dbManager)
        return false;

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT COUNT(*) FROM users WHERE id = ? ";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

bool UserDAO::existByEmail(const std::string& email) {
    if (!_dbManager)
        return false;

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT COUNT(*) FROM users WHERE email = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);

    bool exists = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = sqlite3_column_int(stmt, 0) > 0;
    }

    sqlite3_finalize(stmt);
    return exists;
}

std::vector<std::shared_ptr<UserDTO>> UserDAO::getAllUsers() {
    std::vector<std::shared_ptr<UserDTO>> result;

    if (!_dbManager)
        return result;

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, email, password, type, balance FROM users";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        double balance = sqlite3_column_double(stmt, 5);

        auto user = createUserFromRow(id, name, email, password, type, balance);
        if (user) {
            result.push_back(user);
        }
    }

    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::shared_ptr<UserDTO>> UserDAO::getUsersByType(const std::string& type) {
    std::vector<std::shared_ptr<UserDTO>> result;

    if (!_dbManager)
        return result;

    sqlite3* db = _dbManager->getConnection();
    sqlite3_stmt* stmt;

    const char* sql = "SELECT id, name, email, password, type, balance FROM users WHERE type = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string typeDb = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        double balance = sqlite3_column_double(stmt, 5);

        auto user = createUserFromRow(id, name, email, password, typeDb, balance);
        if (user) {
            result.push_back(user);
        }
    }

    sqlite3_finalize(stmt);
    return result;
}
