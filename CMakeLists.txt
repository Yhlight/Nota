cmake_minimum_required(VERSION 3.10)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Core Library
add_library(libnota src/lib/Lexer.cpp src/lib/Lexer.h src/lib/Token.h)
target_include_directories(libnota PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)

# Main Executable
add_executable(nota src/app/main.cpp)
target_link_libraries(nota libnota)

# Tests
add_executable(nota_tests tests/main.cpp tests/LexerTests.cpp)
target_link_libraries(nota_tests libnota GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
