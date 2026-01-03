#ifndef USER_DAO_H
#define USER_DAO_H

#include <expected>
#include <memory>
#include <string>
#include <vector>

class UserDTO;
class DatabaseManager;

class UserDAO {
   private:
    static std::shared_ptr<DatabaseManager> _dbManager;

    // Helper to reconstruct UserDTO from database row
    static std::shared_ptr<UserDTO>
    createUserFromRow(const std::string& id, const std::string& name, const std::string& email,
                      const std::string& password, const std::string& type, double balance = 0.0);

   public:
    static void setDatabaseManager(std::shared_ptr<DatabaseManager> dbManager);

    static std::expected<std::shared_ptr<UserDTO>, std::string>
    getUserByEmail(const std::string& email);

    static std::expected<std::shared_ptr<UserDTO>, std::string> getUserById(const std::string& id);

    static std::expected<void, std::string> addUser(const std::shared_ptr<UserDTO>& user);

    static std::expected<void, std::string> updateUser(const std::shared_ptr<UserDTO>& user);

    static bool existById(const std::string& id);
    static bool existByEmail(const std::string& email);

    static std::vector<std::shared_ptr<UserDTO>> getAllUsers();
    static std::vector<std::shared_ptr<UserDTO>> getUsersByType(const std::string& type);
};

#endif
