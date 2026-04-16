cmake_minimum_required(VERSION 3.12)

project(rtl_test VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

set(RTL_TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/as_const.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/construct_at.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/expected.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/string_view.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/span.cc"
)

# Catch 2 For Testing
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0 # or latest version
)

FetchContent_MakeAvailable(Catch2)


add_executable("${PROJECT_NAME}" "${RTL_TEST_SOURCES}")

# Link RTL
target_link_libraries("${PROJECT_NAME}" PUBLIC rtl)

# Link Catch 2
target_link_libraries("${PROJECT_NAME}" PRIVATE Catch2::Catch2WithMain)
