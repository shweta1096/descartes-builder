cmake_minimum_required(VERSION 3.5)

project(
  descartesBuilder
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test)

include_directories(src)

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "src/*.h")
file(GLOB_RECURSE PROJECT_RESOURCES "resources/*.qrc")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  qt_add_executable(descartesBuilder MANUAL_FINALIZATION main.cpp
                    ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_RESOURCES})
  # Define target properties for Android with Qt 6 as: set_property(TARGET
  # descartesBuilder APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
  # ${CMAKE_CURRENT_SOURCE_DIR}/android) For more information, see
  # https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
  add_executable(descartesBuilder main.cpp ${PROJECT_SOURCES}
                                  ${PROJECT_HEADERS} ${PROJECT_RESOURCES})
endif()

target_link_libraries(descartesBuilder PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(
  descartesBuilder
  PROPERTIES ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
             ${PROJECT_VERSION} MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} MACOSX_BUNDLE
             TRUE WIN32_EXECUTABLE
             TRUE)

include(GNUInstallDirs)
install(
  TARGETS descartesBuilder
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(descartesBuilder)
endif()

# make library for test to link to
add_library(descartesBuilder_lib SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                                        ${PROJECT_RESOURCES})

target_link_libraries(descartesBuilder_lib
                      PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# unit tests

add_subdirectory(external/googletest)
include(GoogleTest)
include(CTest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(unitTests ${TEST_SOURCES})

target_link_libraries(
  unitTests PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Test
                    gtest descartesBuilder_lib)

gtest_discover_tests(unitTests)
