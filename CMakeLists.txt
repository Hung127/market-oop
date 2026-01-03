cmake_minimum_required(VERSION 3.16)

project(ExclusiveEcommerce VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt path (chỉnh lại nếu Qt cài chỗ khác)
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64" CACHE PATH "Qt path")
endif()

# Qt auto tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

# Executable
add_executable(ExclusiveEcommerce
    src/main.cpp
    src/models/productmodel.h
    src/models/productmodel.cpp
    resources.qrc
)

# Link Qt
target_link_libraries(ExclusiveEcommerce
    PRIVATE Qt6::Core Qt6::Gui Qt6::Quick
)

# Include dirs
target_include_directories(ExclusiveEcommerce PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
)

# MinGW fix
if(MINGW)
    target_compile_options(ExclusiveEcommerce PRIVATE -fno-stack-protector)
    target_link_options(ExclusiveEcommerce PRIVATE -fno-stack-protector)
endif()

# Windows settings
if(WIN32)
    set_target_properties(ExclusiveEcommerce PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
