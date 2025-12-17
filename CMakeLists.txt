cmake_minimum_required(VERSION 3.16)
project(MarketApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
message(STATUS "Project include dir: ${PROJECT_INCLUDE_DIR}")
if(NOT EXISTS "${PROJECT_INCLUDE_DIR}")
  message(FATAL_ERROR "Include dir does not exist: ${PROJECT_INCLUDE_DIR}")
endif()

# Collect sources (exclude main.cpp from core)
file(GLOB_RECURSE CORE_SRC
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DTO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DAO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/BUS/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/UI/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/lib/*.cpp"
)

# If your main is at project root (CMakeLists location) use this path:
set(MAIN_SRC "${CMAKE_SOURCE_DIR}/main.cpp")
# If main is in src/, change the line above to "${CMAKE_SOURCE_DIR}/src/main.cpp"

# Exclude main.cpp from core sources if glob picked it up
list(REMOVE_ITEM CORE_SRC "${MAIN_SRC}")

# Optional: exclude tests folder under src if present
list(FILTER CORE_SRC EXCLUDE REGEX ".*/tests/.*\\.cpp$")

message(STATUS "Core sources found: ${CORE_SRC}")
message(STATUS "Main source: ${MAIN_SRC}")

# Core library
add_library(core STATIC ${CORE_SRC})
target_include_directories(core PUBLIC "${PROJECT_INCLUDE_DIR}")
target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)

# Executable: use main.cpp and link core
add_executable(app ${MAIN_SRC})
target_link_libraries(app PRIVATE core)
target_include_directories(app PRIVATE "${PROJECT_INCLUDE_DIR}")

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
if(TEST_SOURCES)
  add_executable(tests ${TEST_SOURCES})
  target_include_directories(tests PRIVATE "${PROJECT_INCLUDE_DIR}")
  target_link_libraries(tests PRIVATE core gtest_main)
  add_test(NAME unit_tests COMMAND tests)
else()
  message(STATUS "No test sources found in tests/ directory")
endif()
