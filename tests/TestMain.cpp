#include <gtest/gtest.h>

#include "TestDbListener.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // Register our listener that creates a fresh DB per test and wires DAOs.
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();

    // Append our custom listener. The default printer remains.
    listeners.Append(new TestDbListener());

    // Do NOT use the old global DB environment to avoid a single DB for all tests.
    // ::testing::AddGlobalTestEnvironment(new GlobalDbEnvironment());  // removed

    return RUN_ALL_TESTS();
}
