#ifndef TEST_ENVIRONMENT_H
#define TEST_ENVIRONMENT_H

#include <memory>
#include <string>

#include <gtest/gtest.h>

class DatabaseManager;
class ProductRepository;

/**
 * Global test environment that initializes a shared SQLite test database
 * and wires it into DAO classes before any tests run.
 */
class GlobalDbEnvironment : public ::testing::Environment {
   public:
    GlobalDbEnvironment() = default;
    ~GlobalDbEnvironment() override = default;

    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<DatabaseManager> getDbManager() const;
    std::shared_ptr<ProductRepository> getProductRepository() const;

   private:
    std::shared_ptr<DatabaseManager> dbManager;
    std::shared_ptr<ProductRepository> productRepository;  // Repository Pattern: Instance for testing
    std::string testDbPath;
};

#endif  // TEST_ENVIRONMENT_H
