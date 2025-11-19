cmake_minimum_required(VERSION 3.10)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

enable_testing()

add_subdirectory(external/googletest)

add_executable(NotaTests tests/main.cpp tests/core/test_nota_value.cpp tests/lexer/test_lexer.cpp tests/parser/test_parser.cpp)
target_link_libraries(NotaTests PRIVATE gtest gmock NotaCore)
