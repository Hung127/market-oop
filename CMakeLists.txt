cmake_minimum_required(VERSION 3.16)

project(ExclusiveEcommerce VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set Qt6 path
if(NOT DEFINED CMAKE_PREFIX_PATH)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64" CACHE PATH "Path to Qt installation")
endif()

# Auto MOC, RCC, UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick)

# Disable stack protector for MinGW
if(MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector")
endif()

# Create resources.qrc file with all QML files
set(QRC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc)
file(WRITE ${QRC_FILE} "<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n")

# Add QML files
file(WRITE ${QRC_FILE} "<!DOCTYPE RCC>\n<RCC version=\"1.0\">\n<qresource prefix=\"/qml\">\n")
file(GLOB QML_FILES "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml")
foreach(qml_file ${QML_FILES})
    file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${qml_file})
    get_filename_component(qml_name ${qml_file} NAME)
    file(APPEND ${QRC_FILE} "    <file alias=\"${qml_name}\">${rel_path}</file>\n")
endforeach()
file(APPEND ${QRC_FILE} "</qresource>\n")

# Add icons
file(APPEND ${QRC_FILE} "<qresource prefix=\"/icons\">\n")
file(GLOB ICON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/*")
foreach(icon_file ${ICON_FILES})
    if(EXISTS ${icon_file} AND NOT IS_DIRECTORY ${icon_file})
        file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${icon_file})
        get_filename_component(icon_name ${icon_file} NAME)
        file(APPEND ${QRC_FILE} "    <file alias=\"${icon_name}\">${rel_path}</file>\n")
    endif()
endforeach()
file(APPEND ${QRC_FILE} "</qresource>\n")

# Add images
file(APPEND ${QRC_FILE} "<qresource prefix=\"/images\">\n")
file(GLOB IMAGE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/resources/image/*")
foreach(image_file ${IMAGE_FILES})
    if(EXISTS ${image_file} AND NOT IS_DIRECTORY ${image_file})
        file(RELATIVE_PATH rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${image_file})
        get_filename_component(image_name ${image_file} NAME)
        file(APPEND ${QRC_FILE} "    <file alias=\"${image_name}\">${rel_path}</file>\n")
    endif()
endforeach()
file(APPEND ${QRC_FILE} "</qresource>\n")

file(APPEND ${QRC_FILE} "</RCC>\n")

# Create executable with resources
add_executable(ExclusiveEcommerce
    src/main.cpp
    src/models/productmodel.h
    src/models/productmodel.cpp
    resources.qrc
)

# Link Qt libraries
target_link_libraries(ExclusiveEcommerce PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
)

# Add include directories so headers in src/models, src/services and src/controllers are found
target_include_directories(ExclusiveEcommerce PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controllers
)

# Fix stack protector for MinGW
if(MINGW)
    target_compile_options(ExclusiveEcommerce PRIVATE -fno-stack-protector)
    target_link_options(ExclusiveEcommerce PRIVATE -fno-stack-protector)
endif()

# Windows specific settings
if(WIN32)
    set_target_properties(ExclusiveEcommerce PROPERTIES
        WIN32_EXECUTABLE TRUE
    )

    # Deploy Qt dependencies
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
