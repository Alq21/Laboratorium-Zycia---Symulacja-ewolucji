cmake_minimum_required(VERSION 3.16)

project(LifeLabolatory VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")
file(GLOB FORMS "*.ui")

#
add_executable(LifeLabolatory ${SOURCES} ${HEADERS} ${FORMS})

target_include_directories(LifeLabolatory PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_include_directories(LifeLabolatory PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}

    ${CMAKE_CURRENT_BINARY_DIR}
)

add_compile_definitions(SOURCE_DIR="${CMAKE_SOURCE_DIR}")
configure_file(${CMAKE_SOURCE_DIR}/example_config.json
               ${CMAKE_BINARY_DIR}/example_config.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/improved_config.json
               ${CMAKE_BINARY_DIR}/improved_config.json COPYONLY)

target_link_libraries(LifeLabolatory PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.LifeLabolatory)
endif()
set_target_properties(LifeLabolatory PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS LifeLabolatory
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LifeLabolatory)
endif()
