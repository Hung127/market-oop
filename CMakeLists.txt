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
list(FILTER CORE_SRC EXCLUDE REGEX ".*/tests/.*\\.cpp$")

message(STATUS "Core sources found: ${CORE_SRC}")
message(STATUS "Main source: ${MAIN_SRC}")

# -----------------------
# Core library
# -----------------------
add_library(core STATIC ${CORE_SRC})
target_include_directories(core PUBLIC "${PROJECT_INCLUDE_DIR}")

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

# -----------------------
# Tests (GoogleTest) via FetchContent
# -----------------------
if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
  )
  # Keep gtest using same CRT on Windows
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
  if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_include_directories(tests PRIVATE "${PROJECT_INCLUDE_DIR}")
    target_link_libraries(tests PRIVATE core gtest_main)

    # Discover and register individual GoogleTest cases with CTest
    include(GoogleTest)
    gtest_discover_tests(tests
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      # You can add DISCOVERY_TIMEOUT or TEST_PROPERTIES if needed
      # PROPERTIES TIMEOUT 120
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
# ctest will now see each GoogleTest test case as a separate CTest test.
