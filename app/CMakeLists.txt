cmake_minimum_required(VERSION 3.8)

project(
  DescartesBuilder
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

add_subdirectory(external/qtnodes)

include_directories(src)

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "src/*.hpp")
file(GLOB_RECURSE PROJECT_RESOURCES "resources/*.qrc")

qt_add_executable(DescartesBuilder MANUAL_FINALIZATION main.cpp
                  ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_RESOURCES})
# Define target properties for Android with Qt 6 as: set_property(TARGET
# DescartesBuilder APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
# https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

target_link_libraries(DescartesBuilder PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                               QtNodes)

set_target_properties(
  DescartesBuilder
  PROPERTIES ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
             ${PROJECT_VERSION} MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} MACOSX_BUNDLE
             TRUE WIN32_EXECUTABLE
             TRUE)

include(GNUInstallDirs)
install(
  TARGETS DescartesBuilder
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(DescartesBuilder)
endif()

# make library for test to link to
add_library(DescartesBuilder_lib SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                                        ${PROJECT_RESOURCES})

target_link_libraries(DescartesBuilder_lib
                      PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QtNodes)
# unit tests

add_subdirectory(external/googletest)
include(GoogleTest)
include(CTest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(unitTests ${TEST_SOURCES})

target_link_libraries(
  unitTests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test
                    gtest DescartesBuilder_lib QtNodes)

gtest_discover_tests(unitTests)
