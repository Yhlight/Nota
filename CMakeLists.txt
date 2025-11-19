cmake_minimum_required(VERSION 3.10)

project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(nota src/main.cpp)

enable_testing()
add_executable(nota_tests tests/test_main.cpp)
add_test(NAME nota_tests COMMAND nota_tests)
