cmake_minimum_required(VERSION 3.14)
project(Nota)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ##############################################################################
# Nota Library
# ##############################################################################
add_library(nota_lib STATIC)

target_include_directories(nota_lib
    PUBLIC ${PROJECT_SOURCE_DIR}/src/lib
)

target_sources(nota_lib PRIVATE
    src/lib/lexer/Lexer.cpp
)

# ##############################################################################
# Nota Executable
# ##############################################################################
add_executable(nota src/app/main.cpp)

target_link_libraries(nota PRIVATE nota_lib)

# ##############################################################################
# Testing
# ##############################################################################
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(tests)
