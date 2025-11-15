cmake_minimum_required(VERSION 3.10)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(nota_lib
    src/lexer.cpp
    src/parser.cpp
    src/ast.hpp
)

add_subdirectory(tests)
