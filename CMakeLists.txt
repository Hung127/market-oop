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

# Option: control whether to require/use libsodium
option(USE_SODIUM "Enable libsodium support (password hashing with crypto_pwhash)" ON)

# Collect sources (exclude main.cpp from core)
file(GLOB_RECURSE CORE_SRC
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DTO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DAO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/BUS/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/UI/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Utils/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Utils/BUS/*.cpp"
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

# Recommended: enable warnings for core (you can adjust per-compiler)
target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)

# Find and link libsodium if requested
if(USE_SODIUM)
  # Try pkg-config first (if available), then fallback to find_library
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SODIUM_PKG libsodium)
  endif()

  if(DEFINED SODIUM_PKG_LIBRARIES AND NOT "${SODIUM_PKG_LIBRARIES}" STREQUAL "")
    message(STATUS "libsodium found via pkg-config")
    target_include_directories(core PRIVATE ${SODIUM_PKG_INCLUDE_DIRS})
    # Use PUBLIC so consumers of core (app/tests) inherit the link dependency
    target_link_libraries(core PUBLIC ${SODIUM_PKG_LIBRARIES})
    # Add any extra flags provided by pkg-config (if any)
    if(DEFINED SODIUM_PKG_CFLAGS_OTHER)
      target_compile_options(core PRIVATE ${SODIUM_PKG_CFLAGS_OTHER})
    endif()
  else()
    find_library(SODIUM_LIB sodium)
    if(SODIUM_LIB)
      message(STATUS "libsodium found: ${SODIUM_LIB}")
      # Link as PUBLIC so executables/tests linking core will also link sodium
      target_link_libraries(core PUBLIC ${SODIUM_LIB})
    else()
      message(FATAL_ERROR "libsodium not found. Install libsodium-dev (Linux) / libsodium (Homebrew) or set -DUSE_SODIUM=OFF")
    endif()
  endif()
else()
  message(STATUS "USE_SODIUM=OFF -- libsodium support disabled")
endif()

# Executable: use main.cpp and link core
add_executable(app ${MAIN_SRC})
target_link_libraries(app PRIVATE core)
target_include_directories(app PRIVATE "${PROJECT_INCLUDE_DIR}")

# GoogleTest (FetchContent)
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
