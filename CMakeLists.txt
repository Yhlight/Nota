cmake_minimum_required(VERSION 3.10)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nota src/main.cpp src/lexer.cpp src/parser.cpp)

# Tests
enable_testing()

add_executable(test_lexer tests/test_lexer.cpp src/lexer.cpp)
add_test(NAME test_lexer COMMAND test_lexer)

add_executable(test_parser tests/test_parser.cpp src/lexer.cpp src/parser.cpp)
add_test(NAME test_parser COMMAND test_parser)
