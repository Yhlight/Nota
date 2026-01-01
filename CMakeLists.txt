cmake_minimum_required(VERSION 3.20)
project(Nota)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library for core compiler components
add_library(notalib STATIC
    src/lexer/Lexer.cpp
)
target_include_directories(notalib PUBLIC src)

# Main executable
add_executable(nota src/main.cpp)
target_link_libraries(nota notalib)

include(FetchContent)
enable_testing()

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(nota_tests
    tests/main_test.cpp
    tests/lexer_test.cpp
)
target_link_libraries(nota_tests GTest::gtest_main notalib)

include(GoogleTest)
gtest_discover_tests(nota_tests)
