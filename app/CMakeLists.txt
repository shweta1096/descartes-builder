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

add_definitions("-DQT_MESSAGELOGCONTEXT")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Test OpenGL)

add_subdirectory(external/qtnodes)
add_subdirectory(external/qtutility)

include_directories(include)

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")
file(GLOB_RECURSE PROJECT_HEADERS "include/*.hpp")
file(GLOB_RECURSE PROJECT_RESOURCES "resources/*.qrc")

qt_add_executable(DescartesBuilder MANUAL_FINALIZATION main.cpp
                  ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_RESOURCES})

target_link_libraries(
  DescartesBuilder PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                           Qt${QT_VERSION_MAJOR}::OpenGL QtNodes QtUtility)

target_compile_options(
  DescartesBuilder
  PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4
          /wd4127
          /EHsc
          /utf-8>
          $<$<CXX_COMPILER_ID:GNU>:-Wall
          -Wextra>
          $<$<CXX_COMPILER_ID:AppleClang>:-Wall
          -Wextra
          -Werror>)

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

target_link_libraries(
  DescartesBuilder_lib PUBLIC Qt${QT_VERSION_MAJOR}::Widgets
                              Qt${QT_VERSION_MAJOR}::OpenGL QtNodes QtUtility)

add_subdirectory(external/googletest)
include(GoogleTest)
include(CTest)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

add_executable(unitTests ${TEST_SOURCES})

target_link_libraries(unitTests PRIVATE gtest DescartesBuilder_lib)

gtest_discover_tests(unitTests)
