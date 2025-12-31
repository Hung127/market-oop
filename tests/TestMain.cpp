#include <gtest/gtest.h>

#include "TestEnvironment.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Register global environment (will run SetUp() before any tests)
    ::testing::AddGlobalTestEnvironment(new GlobalDbEnvironment());

    return RUN_ALL_TESTS();
}
