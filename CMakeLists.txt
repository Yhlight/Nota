cmake_minimum_required(VERSION 3.10)
project(Nota)

# Set the C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create the core library (header-only)
add_library(nota_core INTERFACE)
target_include_directories(nota_core INTERFACE src/core)

# Add the main executable
add_executable(nota src/main.cpp)
target_link_libraries(nota PRIVATE nota_core)

# Add the tests subdirectory
add_subdirectory(tests)
