cmake_minimum_required(VERSION 3.8)

project(QtNodesLibrary)

set(QTNODES_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(QTNODES_DEBUG_LIB_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/libQtNodes.dylib")
set(QTNODES_RELEASE_LIB_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/release/libQtNodes.dylib")

add_library(QtNodes INTERFACE)
target_include_directories(QtNodes INTERFACE ${QTNODES_INCLUDE_DIR})
target_link_libraries(
  QtNodes INTERFACE $<$<CONFIG:Debug>:${QTNODES_DEBUG_LIB_PATH}>
                    $<$<CONFIG:Release>:${QTNODES_RELEASE_LIB_PATH}>)
