cmake_minimum_required(VERSION 3.12)
project(rtl VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library("${PROJECT_NAME}" INTERFACE)
target_include_directories("${PROJECT_NAME}"
    INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_subdirectory(test)
