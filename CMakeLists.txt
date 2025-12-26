cmake_minimum_required(VERSION 3.16)
project(ExclusiveEcommerce VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Qml Quick)

add_executable(ExclusiveEcommerceApp
    src/main.cpp
    src/productmodel.cpp
    src/productmodel.h
    qml.qrc
)

target_link_libraries(ExclusiveEcommerceApp PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Qml
)

set_target_properties(ExclusiveEcommerceApp PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
