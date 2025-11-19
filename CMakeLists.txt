cmake_minimum_required(VERSION 3.16)
project(MarketApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/main.cpp"
)
# Exclude tests/
list(FILTER SRC_FILES EXCLUDE REGEX ".*/tests/.*\\.cpp$")

add_executable(app ${SRC_FILES})
