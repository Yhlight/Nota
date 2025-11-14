cmake_minimum_required(VERSION 3.16)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_subdirectory(third_party/googletest)

# Create a library for the core Nota components
add_library(nota_lib src/lexer.cpp)

# Create the main executable and link it against the library
add_executable(nota src/main.cpp)
target_link_libraries(nota PUBLIC nota_lib)

add_subdirectory(tests)
