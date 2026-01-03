#ifndef TEST_DB_LISTENER_H
#define TEST_DB_LISTENER_H

#include <memory>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

class DatabaseManager;

/**
 * TestDbListener
 *
 * - OnTestStart: create a fresh sqlite DB for the test, initialize it and wire into DAOs.
 * - OnTestEnd: teardown and remove the DB file, reset DAO database managers.
 *
 * Currently uses per-test-case DB isolation. If you want per-test-file instead,
 * we can change the key used (e.g. to test_info.file()).
 */
class TestDbListener : public ::testing::EmptyTestEventListener {
   public:
    TestDbListener() = default;
    ~TestDbListener() override = default;

    void OnTestStart(const ::testing::TestInfo& test_info) override;
    void OnTestEnd(const ::testing::TestInfo& test_info) override;

   private:
    // key -> pair(dbManager, dbPath)
    std::unordered_map<std::string, std::pair<std::shared_ptr<DatabaseManager>, std::string>>
        _active;
};

#endif  // TEST_DB_LISTENER_H
