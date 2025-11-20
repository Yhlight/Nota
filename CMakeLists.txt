cmake_minimum_required(VERSION 3.10)

project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src/lib)

add_executable(nota src/main.cpp)
target_link_libraries(nota libnota)

enable_testing()
add_executable(nota_tests tests/test_token.cpp tests/test_lexer.cpp)
target_include_directories(nota_tests PUBLIC tests/vendor)
target_link_libraries(nota_tests libnota)
add_test(NAME nota_tests COMMAND nota_tests)
