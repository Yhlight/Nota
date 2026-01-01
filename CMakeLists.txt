cmake_minimum_required(VERSION 3.20)
project(Nota)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nota src/main.cpp)

include(FetchContent)
enable_testing()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(nota_tests tests/main_test.cpp)
target_link_libraries(nota_tests GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
