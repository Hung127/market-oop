cmake_minimum_required(VERSION 3.16)

project(ExclusiveEcommerce VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Quick)

qt_standard_project_setup()

qt_add_executable(ExclusiveEcommerce
    src/main.cpp
    src/productmodel.h
    src/productmodel.cpp
)

qt_add_qml_module(ExclusiveEcommerce
    URI ExclusiveEcommerce
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/HeaderBar.qml
        qml/FooterBar.qml
        qml/CategoryPanel.qml
        qml/HeroBanner.qml
        qml/FlashSaleSection.qml
        qml/FlashSaleTimer.qml
        qml/BestSellingSection.qml
        qml/BrowseCategorySection.qml
        qml/ExploreProductsSection.qml
        qml/ProductCard.qml
        qml/ServicesStrip.qml
    RESOURCES
        qml.qrc
)

target_link_libraries(ExclusiveEcommerce PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
)

set_target_properties(ExclusiveEcommerce PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

install(TARGETS ExclusiveEcommerce
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)