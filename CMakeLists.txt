cmake_minimum_required(VERSION 3.15)
project(Nota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Google Test ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# --- Nota Core Library ---
file(GLOB_RECURSE NOTA_CORE_SOURCES
    "src/core/*.cpp"
    "src/lexer/*.cpp"
    "src/parser/*.cpp"
    "src/analyzer/*.cpp"
    "src/ast/*.cpp"
    "src/codegen/*.cpp"
    "src/codegen/ExpressionEvaluator.cpp"
)
add_library(NotaCore STATIC ${NOTA_CORE_SOURCES})
target_include_directories(NotaCore PUBLIC src)

# --- Main Executable ---
add_executable(Nota src/main.cpp)
target_link_libraries(Nota PRIVATE NotaCore)

# --- Test Executable ---
add_executable(NotaTests)
target_link_libraries(NotaTests PRIVATE gtest_main NotaCore)

# Function to add all test files from a directory
function(add_all_tests test_executable)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        file(GLOB_RECURSE test_files CONFIGURE_DEPENDS "tests/*.cpp")
        if(test_files)
            target_sources(${test_executable} PRIVATE ${test_files})
        endif()
    endif()
endfunction()

add_all_tests(NotaTests)

enable_testing()
add_test(NAME NotaCompilerTests COMMAND NotaTests)
