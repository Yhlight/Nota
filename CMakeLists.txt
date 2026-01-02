cmake_minimum_required(VERSION 3.15)
project(Nota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(NotaCore STATIC
    src/Token.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/SemanticAnalyzer.cpp
    src/SymbolTable.cpp
    src/CodeGenerator.cpp
)

add_executable(Nota src/main.cpp)
target_link_libraries(Nota PRIVATE NotaCore)

# Testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(NotaTests
    tests/LexerTests.cpp
    tests/ParserTests.cpp
    tests/SemanticAnalyzerTests.cpp
    tests/CodeGeneratorTests.cpp
)
target_link_libraries(NotaTests PRIVATE GTest::gtest_main NotaCore)

include(GoogleTest)
gtest_discover_tests(NotaTests)
