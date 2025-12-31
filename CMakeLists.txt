cmake_minimum_required(VERSION 3.16)
project(MarketApp LANGUAGES CXX)

# -----------------------
# Project / policy setup
# -----------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Debug for CI if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# -----------------------
# Options
# -----------------------
option(USE_SODIUM "Enable libsodium support (password hashing with crypto_pwhash)" ON)
option(BUILD_TESTS "Build unit tests" ON)

# -----------------------
# Include dir check
# -----------------------
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
message(STATUS "Project include dir: ${PROJECT_INCLUDE_DIR}")
if(NOT EXISTS "${PROJECT_INCLUDE_DIR}")
  message(FATAL_ERROR "Include dir does not exist: ${PROJECT_INCLUDE_DIR}")
endif()

# -----------------------
# FetchContent helper available early
# -----------------------
include(FetchContent)

# -----------------------
# Locate SQLite3 (required for DB/DAOs)
# -----------------------
# Try system package first (config and module modes). If not found, fetch & build
# the official SQLite amalgamation and create an imported target SQLite::SQLite3.
find_package(SQLite3 QUIET CONFIG)

if (SQLite3_FOUND)
  message(STATUS "Found SQLite3 via find_package (CONFIG)")
elseif (TARGET SQLite::SQLite3)
  message(STATUS "Found SQLite::SQLite3 target already available")
  set(SQLite3_FOUND TRUE)
else()
  find_package(SQLite3 QUIET)
  if (SQLite3_FOUND)
    message(STATUS "Found SQLite3 via find_package (module)")
  endif()
endif()

if (NOT SQLite3_FOUND)
  # Amalgamation version (numeric, no dots). Change to desired SQLite release.
  set(SQLITE_AMALGAMATION_VERSION "3410200" CACHE STRING "SQLite amalgamation version (numeric, no dots)")
  set(SQLITE_AMALGAMATION_URL
      "https://www.sqlite.org/2023/sqlite-amalgamation-${SQLITE_AMALGAMATION_VERSION}.zip"
      CACHE STRING "URL to download sqlite amalgamation zip")

  message(STATUS "SQLite3 not found on system — fetching amalgamation from ${SQLITE_AMALGAMATION_URL}")

  FetchContent_Declare(
    sqlite_amalgamation
    URL ${SQLITE_AMALGAMATION_URL}
    # For security you can pin URL_HASH SHA256=<hash> here
  )
  FetchContent_MakeAvailable(sqlite_amalgamation)

  # Locate sqlite3.c / sqlite3.h in fetched content (top-level or single subdir)
  if (EXISTS "${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c" AND
      EXISTS "${sqlite_amalgamation_SOURCE_DIR}/sqlite3.h")
    set(SQLITE_AMALG_SRC_DIR "${sqlite_amalgamation_SOURCE_DIR}")
  else()
    file(GLOB children RELATIVE "${sqlite_amalgamation_SOURCE_DIR}" "${sqlite_amalgamation_SOURCE_DIR}/*")
    set(SQLITE_AMALG_SRC_DIR "")
    foreach(child ${children})
      if (EXISTS "${sqlite_amalgamation_SOURCE_DIR}/${child}/sqlite3.c" AND
          EXISTS "${sqlite_amalgamation_SOURCE_DIR}/${child}/sqlite3.h")
        set(SQLITE_AMALG_SRC_DIR "${sqlite_amalgamation_SOURCE_DIR}/${child}")
        break()
      endif()
    endforeach()
  endif()

  if (NOT SQLITE_AMALG_SRC_DIR)
    message(FATAL_ERROR "Failed to locate sqlite3.c in fetched amalgamation (looked in ${sqlite_amalgamation_SOURCE_DIR})")
  endif()

  message(STATUS "Using SQLite amalgamation sources from: ${SQLITE_AMALG_SRC_DIR}")

  add_library(sqlite3_amalgamation STATIC
    "${SQLITE_AMALG_SRC_DIR}/sqlite3.c"
  )
  target_include_directories(sqlite3_amalgamation PUBLIC "${SQLITE_AMALG_SRC_DIR}")

  # Enable common extensions you may want; remove if you prefer minimal build.
  # Use symbol names (no -D prefix) — CMake adds -D automatically.
  target_compile_definitions(sqlite3_amalgamation PUBLIC
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_JSON1
    "SQLITE_THREADSAFE=1"
  )

  set_target_properties(sqlite3_amalgamation PROPERTIES POSITION_INDEPENDENT_CODE ON)

  # Provide an alias target matching find_package style
  add_library(SQLite::SQLite3 ALIAS sqlite3_amalgamation)

  set(SQLite3_FOUND TRUE)
  message(STATUS "Built SQLite3 from amalgamation and created target SQLite::SQLite3")
endif()

# -----------------------
# Sources
# -----------------------
file(GLOB_RECURSE CORE_SRC
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DTO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DAO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/BUS/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/UI/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Utils/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Utils/BUS/*.cpp"
)

set(MAIN_SRC "${CMAKE_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM CORE_SRC "${MAIN_SRC}")
# Exclude any test sources accidentally under src
list(FILTER CORE_SRC EXCLUDE REGEX ".*/tests/.*\\.cpp$")

message(STATUS "Core sources found: ${CORE_SRC}")
message(STATUS "Main source: ${MAIN_SRC}")

# -----------------------
# Core library
# -----------------------
add_library(core STATIC ${CORE_SRC})
target_include_directories(core PUBLIC "${PROJECT_INCLUDE_DIR}")

# Link SQLite to core (DAOs + DatabaseManager use sqlite3 API)
target_link_libraries(core PUBLIC SQLite::SQLite3)

if (MSVC)
  target_compile_options(core PRIVATE /W4 /permissive-)
else()
  target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# -----------------------
# libsodium discovery & linking
# -----------------------
if(USE_SODIUM)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SODIUM_PKG libsodium QUIET)
  endif()

  if(DEFINED SODIUM_PKG_LIBRARIES AND NOT "${SODIUM_PKG_LIBRARIES}" STREQUAL "")
    message(STATUS "libsodium found via pkg-config")
    target_include_directories(core PRIVATE ${SODIUM_PKG_INCLUDE_DIRS})
    target_link_libraries(core PUBLIC ${SODIUM_PKG_LIBRARIES})
    if(DEFINED SODIUM_PKG_CFLAGS_OTHER)
      target_compile_options(core PRIVATE ${SODIUM_PKG_CFLAGS_OTHER})
    endif()
  else()
    find_library(SODIUM_LIB sodium)
    find_path(SODIUM_INCLUDE_DIR sodium.h)
    if(SODIUM_LIB AND SODIUM_INCLUDE_DIR)
      message(STATUS "libsodium found: ${SODIUM_LIB}; include: ${SODIUM_INCLUDE_DIR}")
      target_include_directories(core PRIVATE ${SODIUM_INCLUDE_DIR})
      target_link_libraries(core PUBLIC ${SODIUM_LIB})
    else()
      message(FATAL_ERROR "libsodium not found. Install libsodium-dev (Ubuntu) / libsodium (Homebrew) or set -DUSE_SODIUM=OFF")
    endif()
  endif()
else()
  message(STATUS "USE_SODIUM=OFF -- libsodium support disabled")
endif()

# -----------------------
# Application executable
# -----------------------
add_executable(app ${MAIN_SRC})
target_link_libraries(app PRIVATE core)
target_include_directories(app PRIVATE "${PROJECT_INCLUDE_DIR}")

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data")

# Copy assets & data directories to build tree if they exist
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
  file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

# Define PROJECT_ROOT_DIR for code
target_compile_definitions(core PUBLIC 
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

# -----------------------
# Tests (GoogleTest) via FetchContent (robust)
# -----------------------
if(BUILD_TESTS)
  enable_testing()

  # Prefer stable release tag for gtest (avoid main)

    set(GTEST_TAG "release-1.14.0")            # Preferred stable tag
    set(GTEST_ZIP_TAG_URL "https://github.com/google/googletest/archive/refs/tags/${GTEST_TAG}.zip")
    set(GTEST_ZIP_MAIN_URL "https://github.com/google/googletest/archive/refs/heads/main.zip")

    # Try release ZIP first (more reproducible)
    file(DOWNLOAD "${GTEST_ZIP_TAG_URL}" "${CMAKE_BINARY_DIR}/gtest_tag.zip" STATUS dl_status SHOW_PROGRESS)
    list(GET dl_status 0 dl_code)
    if (dl_code EQUAL 0)
      message(STATUS "Downloaded googletest release zip (${GTEST_TAG})")
      FetchContent_Declare(googletest URL "${GTEST_ZIP_TAG_URL}")
    else()
      message(WARNING "Failed to download googletest release zip, falling back to main branch")
      FetchContent_Declare(googletest URL "${GTEST_ZIP_MAIN_URL}")
    endif()

  FetchContent_MakeAvailable(googletest)

  # Collect test sources. Ensure you have exactly one main() in tests/ (TestMain.cpp).
  file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
  if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_include_directories(tests PRIVATE "${PROJECT_INCLUDE_DIR}" "${CMAKE_SOURCE_DIR}/tests")

    # We provide our own TestMain.cpp, so link GTest::gtest (not gtest_main).
    find_package(Threads REQUIRED)
    target_link_libraries(tests PRIVATE core GTest::gtest Threads::Threads)

    if (MSVC)
      target_compile_options(tests PRIVATE /W4 /permissive-)
    else()
      target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    # Discover and register individual GoogleTest cases with CTest
    include(GoogleTest)
    gtest_discover_tests(tests
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
  else()
    message(STATUS "No test sources found in tests/ directory")
  endif()
endif()

# -----------------------
# Install (optional)
# -----------------------
install(TARGETS core app
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# -----------------------
# CI notes
# -----------------------
# Example GitHub Actions steps (Ubuntu) to configure, build and run tests:
# - name: Configure (CMake)
#   run: |
#     mkdir -p build && cd build
#     cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DUSE_SODIUM=ON
# - name: Build
#   run: cmake --build build -- -j$(nproc)
# - name: Run tests
#   run: cd build && ctest --output-on-failure
#
# Notes:
# - Keep exactly one test main in tests/ (TestMain.cpp). The application main (main.cpp) is kept out of the 'core' library.
# - Tests link GTest::gtest (not gtest_main) so your TestMain.cpp provides the runner.
