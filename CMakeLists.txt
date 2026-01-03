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
option(USE_QML "Enable QML UI support" ON)

# -----------------------
# Qt5 Setup (MUST come before any Qt-related code)
# -----------------------
if(USE_QML)
  # Qt auto tools - MUST be set before add_executable/add_library
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTORCC ON)
  set(CMAKE_AUTOUIC ON)

  # Debug: Show MOC status
  message(STATUS "CMAKE_AUTOMOC: ${CMAKE_AUTOMOC}")
  message(STATUS "CMAKE_AUTORCC: ${CMAKE_AUTORCC}")

  # Find all required Qt5 modules
  find_package(Qt5 REQUIRED COMPONENTS 
    Core 
    Gui 
    Quick 
    Qml
    Widgets
    QuickControls2
  )
  
  if(Qt5_FOUND)
    message(STATUS "✓ Qt5 found - QML support enabled")
    message(STATUS "  Qt5 version: ${Qt5_VERSION}")
    message(STATUS "  Qt5Core:  ${Qt5Core_DIR}")
    message(STATUS "  Qt5Quick: ${Qt5Quick_DIR}")
    message(STATUS "  Qt5Qml: ${Qt5Qml_DIR}")
    message(STATUS "  Qt5Widgets: ${Qt5Widgets_DIR}")
  else()
    message(FATAL_ERROR "Qt5 not found.  Install with: sudo dnf install qt5-qtbase-devel qt5-qtdeclarative-devel qt5-qtquickcontrols2-devel")
  endif()
  
  # Set Qt5 specific compiler flags
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
  
endif()

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
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(SQLITE3_PKG sqlite3 QUIET)
  if(SQLITE3_PKG_FOUND)
    message(STATUS "✓ Found SQLite3 via pkg-config:  ${SQLITE3_PKG_VERSION}")
    add_library(SQLite::SQLite3 INTERFACE IMPORTED)
    set_target_properties(SQLite::SQLite3 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${SQLITE3_PKG_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${SQLITE3_PKG_LIBRARIES}"
    )
    set(SQLite3_FOUND TRUE)
  endif()
endif()

if(NOT SQLite3_FOUND)
  find_package(SQLite3 QUIET CONFIG)
  if(SQLite3_FOUND)
    message(STATUS "✓ Found SQLite3 via find_package (CONFIG)")
  elseif(TARGET SQLite::SQLite3)
    message(STATUS "✓ Found SQLite:: SQLite3 target already available")
    set(SQLite3_FOUND TRUE)
  else()
    find_package(SQLite3 QUIET)
    if(SQLite3_FOUND)
      message(STATUS "✓ Found SQLite3 via find_package (module)")
    endif()
  endif()
endif()

if(NOT SQLite3_FOUND)
  set(SQLITE_AMALGAMATION_VERSION "3410200" CACHE STRING "SQLite amalgamation version (numeric, no dots)")
  set(SQLITE_AMALGAMATION_URL
      "https://www.sqlite.org/2023/sqlite-amalgamation-${SQLITE_AMALGAMATION_VERSION}.zip"
      CACHE STRING "URL to download sqlite amalgamation zip")

  message(STATUS "SQLite3 not found on system — fetching amalgamation from ${SQLITE_AMALGAMATION_URL}")

  FetchContent_Declare(
    sqlite_amalgamation
    URL ${SQLITE_AMALGAMATION_URL}
  )
  FetchContent_MakeAvailable(sqlite_amalgamation)

  if(EXISTS "${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c" AND
      EXISTS "${sqlite_amalgamation_SOURCE_DIR}/sqlite3.h")
    set(SQLITE_AMALG_SRC_DIR "${sqlite_amalgamation_SOURCE_DIR}")
  else()
    file(GLOB children RELATIVE "${sqlite_amalgamation_SOURCE_DIR}" "${sqlite_amalgamation_SOURCE_DIR}/*")
    set(SQLITE_AMALG_SRC_DIR "")
    foreach(child ${children})
      if(EXISTS "${sqlite_amalgamation_SOURCE_DIR}/${child}/sqlite3.c" AND
          EXISTS "${sqlite_amalgamation_SOURCE_DIR}/${child}/sqlite3.h")
        set(SQLITE_AMALG_SRC_DIR "${sqlite_amalgamation_SOURCE_DIR}/${child}")
        break()
      endif()
    endforeach()
  endif()

  if(NOT SQLITE_AMALG_SRC_DIR)
    message(FATAL_ERROR "Failed to locate sqlite3.c in fetched amalgamation")
  endif()

  message(STATUS "Using SQLite amalgamation sources from: ${SQLITE_AMALG_SRC_DIR}")

  add_library(sqlite3_amalgamation STATIC "${SQLITE_AMALG_SRC_DIR}/sqlite3.c")
  target_include_directories(sqlite3_amalgamation PUBLIC "${SQLITE_AMALG_SRC_DIR}")
  target_compile_definitions(sqlite3_amalgamation PUBLIC
    SQLITE_ENABLE_FTS5
    SQLITE_ENABLE_JSON1
    "SQLITE_THREADSAFE=1"
  )
  set_target_properties(sqlite3_amalgamation PROPERTIES POSITION_INDEPENDENT_CODE ON)
  add_library(SQLite::SQLite3 ALIAS sqlite3_amalgamation)
  set(SQLite3_FOUND TRUE)
  message(STATUS "✓ Built SQLite3 from amalgamation")
endif()

# -----------------------
# Collect all source files
# -----------------------
# Backend sources (non-Qt)
file(GLOB_RECURSE BACKEND_SRC
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DTO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/DAO/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/BUS/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/database/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/Utils/*.cpp"
)

# Filter out any test files
list(FILTER BACKEND_SRC EXCLUDE REGEX ".*/tests/.*\\.cpp$")

# Qt-related sources AND headers (only if USE_QML is ON)
set(QT_SOURCES "")
set(QT_HEADERS "")
if(USE_QML)
  # Qt Models
  file(GLOB_RECURSE QT_MODEL_SRC "${CMAKE_SOURCE_DIR}/src/models/*.cpp")
  file(GLOB_RECURSE QT_MODEL_HDR "${CMAKE_SOURCE_DIR}/include/models/*.h")
  
  # Qt Controllers (CRITICAL: Must include headers for MOC)
  file(GLOB_RECURSE QT_CONTROLLER_SRC "${CMAKE_SOURCE_DIR}/src/controllers/*.cpp")
  file(GLOB_RECURSE QT_CONTROLLER_HDR "${CMAKE_SOURCE_DIR}/include/controllers/*.h")
  
  # Qt UI components (if any)
  file(GLOB_RECURSE QT_UI_SRC "${CMAKE_SOURCE_DIR}/src/UI/*.cpp")
  file(GLOB_RECURSE QT_UI_HDR "${CMAKE_SOURCE_DIR}/include/UI/*.h")
  
  list(APPEND QT_SOURCES ${QT_MODEL_SRC} ${QT_CONTROLLER_SRC} ${QT_UI_SRC})
  list(APPEND QT_HEADERS ${QT_MODEL_HDR} ${QT_CONTROLLER_HDR} ${QT_UI_HDR})
  
  # Filter out tests
  list(FILTER QT_SOURCES EXCLUDE REGEX ". */tests/.*\\.cpp$")
  list(FILTER QT_HEADERS EXCLUDE REGEX ".*/tests/.*\\.h$")
  
  message(STATUS "Qt Controller Headers: ${QT_CONTROLLER_HDR}")
  message(STATUS "Qt Controller Sources: ${QT_CONTROLLER_SRC}")
endif()

# Combine all sources
set(CORE_SRC ${BACKEND_SRC} ${QT_SOURCES})

# Main source file
set(MAIN_SRC "${CMAKE_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM CORE_SRC "${MAIN_SRC}")

message(STATUS "Backend sources found:  ${BACKEND_SRC}")
if(USE_QML)
  message(STATUS "Qt sources found: ${QT_SOURCES}")
  message(STATUS "Qt headers found:  ${QT_HEADERS}")
endif()
message(STATUS "Main source: ${MAIN_SRC}")

# -----------------------
# QML Resources (if USE_QML is ON)
# -----------------------
set(QML_RESOURCES "")
if(USE_QML)
  if(EXISTS "${CMAKE_SOURCE_DIR}/resources.qrc")
    set(QML_RESOURCES "${CMAKE_SOURCE_DIR}/resources.qrc")
    message(STATUS "✓ QML resources file found:  ${QML_RESOURCES}")
  else()
    message(WARNING "resources.qrc not found - QML resources will not be available")
  endif()
endif()

# -----------------------
# Core library (WITH headers for MOC!)
# -----------------------
set(CORE_ALL_FILES ${CORE_SRC})
if(USE_QML)
  list(APPEND CORE_ALL_FILES ${QT_HEADERS})
endif()

add_library(core STATIC ${CORE_ALL_FILES})

# Include directories for core
target_include_directories(core PUBLIC 
  "${PROJECT_INCLUDE_DIR}"
  "${PROJECT_INCLUDE_DIR}/UI"
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/UI"
)

# Link SQLite to core
target_link_libraries(core PUBLIC SQLite::SQLite3)

# Link Qt5 to core if QML is enabled
if(USE_QML)
  # Link Qt5 libraries
  target_link_libraries(core PUBLIC 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Quick
    Qt5::Qml
    Qt5::Widgets
    Qt5::QuickControls2
  )
  
  # Add Qt5 include directories
  target_include_directories(core PUBLIC
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
    ${Qt5Quick_INCLUDE_DIRS}
    ${Qt5Qml_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5QuickControls2_INCLUDE_DIRS}
    "${CMAKE_SOURCE_DIR}/include/UI"
    "${CMAKE_SOURCE_DIR}/src/UI"
  )
  
  # Add Qt5 compile definitions
  target_compile_definitions(core PUBLIC
    ${Qt5Core_DEFINITIONS}
    ${Qt5Gui_DEFINITIONS}
    ${Qt5Quick_DEFINITIONS}
    ${Qt5Qml_DEFINITIONS}
    ${Qt5Widgets_DEFINITIONS}
    ${Qt5QuickControls2_DEFINITIONS}
  )
  
  # Set position independent code (required for Qt on some platforms)
  set_target_properties(core PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Compiler warnings
target_compile_options(core PRIVATE -Wall -Wextra -Wpedantic)

# -----------------------
# libsodium discovery & linking
# -----------------------
if(USE_SODIUM)
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SODIUM_PKG libsodium QUIET)
  endif()

  if(SODIUM_PKG_FOUND)
    message(STATUS "✓ libsodium found via pkg-config:  ${SODIUM_PKG_VERSION}")
    target_include_directories(core PRIVATE ${SODIUM_PKG_INCLUDE_DIRS})
    target_link_libraries(core PUBLIC ${SODIUM_PKG_LIBRARIES})
    target_link_directories(core PUBLIC ${SODIUM_PKG_LIBRARY_DIRS})
    if(SODIUM_PKG_CFLAGS_OTHER)
      target_compile_options(core PRIVATE ${SODIUM_PKG_CFLAGS_OTHER})
    endif()
  else()
    find_library(SODIUM_LIB sodium)
    find_path(SODIUM_INCLUDE_DIR sodium.h)
    if(SODIUM_LIB AND SODIUM_INCLUDE_DIR)
      message(STATUS "✓ libsodium found:  ${SODIUM_LIB}")
      target_include_directories(core PRIVATE ${SODIUM_INCLUDE_DIR})
      target_link_libraries(core PUBLIC ${SODIUM_LIB})
    else()
      message(FATAL_ERROR "libsodium not found. Install with: sudo dnf install libsodium-devel")
    endif()
  endif()
else()
  message(STATUS "USE_SODIUM=OFF -- libsodium support disabled")
endif()

# -----------------------
# Application executable (ADD resources here!)
# -----------------------
if(USE_QML AND QML_RESOURCES)
  # Add QML resources directly to executable
  add_executable(app ${MAIN_SRC} ${QML_RESOURCES})
else()
  add_executable(app ${MAIN_SRC})
endif()

target_link_libraries(app PRIVATE core)
target_include_directories(app PRIVATE "${PROJECT_INCLUDE_DIR}")

# If using QML, link Qt5 to the app as well
if(USE_QML)
  target_link_libraries(app PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
    Qt5::Widgets
  )
endif()

# Set as GUI application on Windows
if(WIN32 AND USE_QML)
  set_target_properties(app PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

# -----------------------
# Create necessary directories
# -----------------------
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/data")

# Copy assets & data directories to build tree
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/assets")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
  message(STATUS "✓ Copied assets to build directory")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data")
  file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/data" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
  message(STATUS "✓ Copied data to build directory")
endif()

# Copy QML files to build directory for development
if(USE_QML)
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qml")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/qml" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    message(STATUS "✓ Copied QML files to build directory")
  else()
    message(WARNING "qml/ directory not found")
  endif()
  
  # Also copy images if they exist
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/images")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/images" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
    message(STATUS "✓ Copied images to build directory")
  endif()
endif()

# Define PROJECT_ROOT_DIR for code
target_compile_definitions(core PUBLIC 
    PROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}"
)

# -----------------------
# Tests (GoogleTest)
# -----------------------
if(BUILD_TESTS)
  enable_testing()

  set(GTEST_TAG "release-1.14. 0")
  set(GTEST_ZIP_TAG_URL "https://github.com/google/googletest/archive/refs/tags/${GTEST_TAG}.zip")
  set(GTEST_ZIP_MAIN_URL "https://github.com/google/googletest/archive/refs/heads/main.zip")

  file(DOWNLOAD "${GTEST_ZIP_TAG_URL}" "${CMAKE_BINARY_DIR}/gtest_tag.zip" STATUS dl_status SHOW_PROGRESS)
  list(GET dl_status 0 dl_code)
  if(dl_code EQUAL 0)
    message(STATUS "✓ Downloaded googletest release zip (${GTEST_TAG})")
    FetchContent_Declare(googletest URL "${GTEST_ZIP_TAG_URL}")
  else()
    message(WARNING "Failed to download googletest release zip, falling back to main branch")
    FetchContent_Declare(googletest URL "${GTEST_ZIP_MAIN_URL}")
  endif()

  FetchContent_MakeAvailable(googletest)

  file(GLOB_RECURSE TEST_SOURCES "${CMAKE_SOURCE_DIR}/tests/*.cpp")
  if(TEST_SOURCES)
    add_executable(tests ${TEST_SOURCES})
    target_include_directories(tests PRIVATE 
      "${PROJECT_INCLUDE_DIR}" 
      "${CMAKE_SOURCE_DIR}/tests"
      "${CMAKE_SOURCE_DIR}/src"
    )

    find_package(Threads REQUIRED)
    target_link_libraries(tests PRIVATE 
      core 
      GTest::gtest 
      Threads::Threads
    )

    target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)

    include(GoogleTest)
    gtest_discover_tests(tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    
    message(STATUS "✓ Tests configured:  ${TEST_SOURCES}")
  else()
    message(STATUS "No test sources found in tests/ directory")
  endif()
endif()

# -----------------------
# Install targets
# -----------------------
install(TARGETS core app
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# Install QML files if using QML
if(USE_QML)
  install(DIRECTORY "${CMAKE_SOURCE_DIR}/qml" 
          DESTINATION share/MarketApp
          OPTIONAL)
endif()

# -----------------------
# Print configuration summary
# -----------------------
message(STATUS "")
message(STATUS "╔═══════════════════════════════════════╗")
message(STATUS "║     Build Configuration Summary      ║")
message(STATUS "╠═══════════════════════════════════════╣")
message(STATUS "║ Build type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "║ C++ Standard:       ${CMAKE_CXX_STANDARD}")
message(STATUS "║ Compiler:           ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "╠═══════════════════════════════════════╣")
message(STATUS "║ Dependencies:")
message(STATUS "║   SQLite3:           ${SQLite3_FOUND}")
message(STATUS "║   libsodium:        ${USE_SODIUM}")
if(USE_QML)
message(STATUS "║   Qt5:              ${Qt5_VERSION}")
message(STATUS "║   QML UI:           Enabled")
else()
message(STATUS "║   QML UI:           Disabled")
endif()
message(STATUS "╠═══════════════════════════════════════╣")
message(STATUS "║ Features:")
message(STATUS "║   Build tests:      ${BUILD_TESTS}")
message(STATUS "║   AUTOMOC:          ${CMAKE_AUTOMOC}")
message(STATUS "║   AUTORCC:          ${CMAKE_AUTORCC}")
message(STATUS "╚═══════════════════════════════════════╝")
message(STATUS "")
