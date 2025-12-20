cmake_minimum_required(VERSION 3.16)

project(ExclusiveEcommerce VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fix cho MSVC compiler để nhận diện C++17 đúng cách
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Set Qt6 path - có thể override bằng -DCMAKE_PREFIX_PATH khi chạy cmake
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64" CACHE PATH "Path to Qt installation")
endif()

# Tự động xử lý MOC, RCC, UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Tìm Qt6 packages
find_package(Qt6 6.0 REQUIRED COMPONENTS Core Gui Quick Qml)

# Disable stack protector để tránh lỗi linking với MinGW
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
endif()

# Tạo executable
qt_add_executable(ExclusiveEcommerce
    src/main.cpp
    src/productmodel.h
    src/productmodel.cpp
)

# Fix stack protector options
if(MINGW)
    target_compile_options(ExclusiveEcommerce PRIVATE -fno-stack-protector)
    target_link_options(ExclusiveEcommerce PRIVATE -fno-stack-protector)
endif()

# Thêm QML module với RESOURCE_PREFIX để giữ đường dẫn cũ
qt_add_qml_module(ExclusiveEcommerce
    URI ExclusiveEcommerce
    VERSION 1.0
    RESOURCE_PREFIX /
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
)

# Tạo file qml.qrc để chứa resources (icons, images)
set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/qml.qrc)
file(WRITE ${QRC_FILE} "<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n<qresource prefix=\"/\">\n")

# Thêm icons
file(GLOB ICON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/*")
foreach(icon_file ${ICON_FILES})
    file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${icon_file})
    get_filename_component(icon_name ${icon_file} NAME)
    file(APPEND ${QRC_FILE} "    <file alias=\"icons/${icon_name}\">${rel_path}</file>\n")
endforeach()

# Thêm images
file(GLOB IMAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/image/*")
foreach(image_file ${IMAGE_FILES})
    file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${image_file})
    get_filename_component(image_name ${image_file} NAME)
    file(APPEND ${QRC_FILE} "    <file alias=\"images/${image_name}\">${rel_path}</file>\n")
endforeach()

file(APPEND ${QRC_FILE} "</qresource>\n</RCC>\n")

# Thêm qml.qrc vào target
target_sources(ExclusiveEcommerce PRIVATE ${QRC_FILE})

# Link libraries
target_link_libraries(ExclusiveEcommerce PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
)

# Set target properties
set_target_properties(ExclusiveEcommerce PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Deploy Qt dependencies (Windows/MinGW)
if(WIN32)
    # Tìm windeployqt
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ExclusiveEcommerce POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --qmldir "${CMAKE_SOURCE_DIR}/qml"
                "$<TARGET_FILE:ExclusiveEcommerce>"
            COMMENT "Running windeployqt..."
        )
    endif()
endif()

# Install
install(TARGETS ExclusiveEcommerce
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
