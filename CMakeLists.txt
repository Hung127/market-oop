cmake_minimum_required(VERSION 3.16)
project(MarketApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# include headers - dùng target-specific form
# Nếu thư mục thực tế là ".include", đổi "include" -> ".include" ở dòng dưới
set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
message(STATUS "Project include dir: ${PROJECT_INCLUDE_DIR}")
if(NOT EXISTS "${PROJECT_INCLUDE_DIR}")
  message(FATAL_ERROR "Include dir does not exist: ${PROJECT_INCLUDE_DIR}")
endif()

# Tìm tất cả file nguồn .cpp dưới src (đệ quy)
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/main.cpp"
)

# Loại trừ thư mục tests nếu cần
list(FILTER SRC_FILES EXCLUDE REGEX ".*/tests/.*\\.cpp$")

message(STATUS "Found ${SRC_FILES}")
# Tạo executable
add_executable(app ${SRC_FILES})

# Đăng ký include dir chỉ cho target app
target_include_directories(app PRIVATE "${PROJECT_INCLUDE_DIR}")

# Tuỳ chọn: bật warnings
target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
