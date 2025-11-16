cmake_minimum_required(VERSION 3.14)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(nota src/main.cpp src/Lexer.cpp src/Parser.cpp)
target_include_directories(nota PUBLIC src)

enable_testing()

add_executable(nota_tests tests/main.cpp tests/parser_test.cpp src/Lexer.cpp src/Parser.cpp)
target_include_directories(nota_tests PUBLIC src)
target_link_libraries(nota_tests GTest::gtest_main)

add_test(NAME nota_tests COMMAND nota_tests)
