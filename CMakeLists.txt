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
# Convenience options
# -----------------------
option(USE_SODIUM "Enable libsodium support (password hashing with crypto_pwhash)" ON)
option(ENABLE_SANITIZERS "Enable Address/Undefined Behavior sanitizers (Debug only)" OFF)
option(BUILD_EXAMPLES "Build example/demo binaries" OFF)
option(BUILD_TESTS "Build unit tests" ON)

# -----------------------
# Include directory check
# -----------------------
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
message(STATUS "Project include dir: ${PROJECT_INCLUDE_DIR}")
if(NOT EXISTS "${PROJECT_INCLUDE_DIR}")
  message(FATAL_ERROR "Include dir does not exist: ${PROJECT_INCLUDE_DIR}")
endif()

# -----------------------
# Source collection
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

# Main executable source (adjust path if your main lives elsewhere)
set(MAIN_SRC "${CMAKE_SOURCE_DIR}/main.cpp")

# Remove main from core sources if glob picked it up
list(REMOVE_ITEM CORE_SRC "${MAIN_SRC}")

# Exclude in-tree test helper sources if any
list(FILTER CORE_SRC EXCLUDE REGEX ".*/tests/.*\\.cpp$")

message(STATUS "Core sources found: ${CORE_SRC}")
message(STATUS "Main source: ${MAIN_SRC}")

# -----------------------
# Core library target
# -----------------------
add_library(core STATIC ${CORE_SRC})
target_include_directories(core PUBLIC "${PROJECT_INCLUDE_DIR}")

# Compiler warnings (adjust per compiler if needed)
if (MSVC)
  target_compile_options(core PRIVATE /W4 /permissive-)
else()
  target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)
  # treat warnings as errors in CI optionally:
  # target_compile_options(core PRIVATE -Werror)
endif()

# -----------------------
# Sanitizers (optional)
# -----------------------
if(ENABLE_SANITIZERS)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "ENABLE_SANITIZERS is ON but build type is not Debug. Sanitizers are intended for Debug.")
  endif()

  if(NOT MSVC)
    message(STATUS "Enabling AddressSanitizer and UndefinedBehaviorSanitizer for core and tests")
    target_compile_options(core PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(core PRIVATE -fsanitize=address,undefined)
  else()
    message(WARNING "Sanitizers are not configured for MSVC in this CMakeLists.")
  endif()
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
    message(STATUS "libsodium found via pkg-config: ${SODIUM_PKG_INCLUDEDIR}")
    target_include_directories(core PRIVATE ${SODIUM_PKG_INCLUDE_DIRS})
    # Link as PUBLIC so consumers of core (app/tests) inherit the dependency
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
    elseif(NOT SODIUM_LIB AND NOT SODIUM_INCLUDE_DIR)
      message(FATAL_ERROR "libsodium not found. Install libsodium-dev (Linux) / libsodium (Homebrew) or set -DUSE_SODIUM=OFF")
    else()
      # partial find - prefer explicit user help
      message(FATAL_ERROR "libsodium found partially (include: ${SODIUM_INCLUDE_DIR}, lib: ${SODIUM_LIB}). Please install libsodium-dev or adjust paths.")
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
# Examples / small demos
# -----------------------
if(BUILD_EXAMPLES)
  # simple demo executable that can exercise password utils
  add_executable(password_demo examples/password_demo.cpp src/Utils/Password.cpp)
  target_include_directories(password_demo PRIVATE "${PROJECT_INCLUDE_DIR}")
  target_link_libraries(password_demo PRIVATE core)
endif()

# -----------------------
# Tests (GoogleTest) via FetchContent
# -----------------------
if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
  )
  # Prevent overriding compiler/linker options on Windows
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
endif()

# -----------------------
# Install / packaging (optional)
# -----------------------
install(TARGETS core app
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# -----------------------
# Helpful messages for CI (GitHub Actions)
# -----------------------
# Notes for GitHub Actions runner (Ubuntu):
# - Ensure libsodium-dev is installed:
#     sudo apt-get update && sudo apt-get install -y libsodium-dev
# - Build in CI:
#     mkdir build && cd build
#     cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SANITIZERS=ON -DUSE_SODIUM=ON -DBUILD_TESTS=ON
#     cmake --build . -- -j$(nproc)
#     ctest --output-on-failure
#
# If you prefer not to require libsodium on CI, set -DUSE_SODIUM=OFF when configuring.
#
# End of CMakeLists
