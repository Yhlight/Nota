cmake_minimum_required(VERSION 3.10)

project(NotaTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib/googletest)

include_directories(
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
    ../src
)

add_executable(nota_tests
    main_test.cpp
    lexer_test.cpp
    parser_test.cpp
    ../src/lexer.cpp
    ../src/parser.cpp
)

target_link_libraries(
    nota_tests
    gtest
    gtest_main
)
