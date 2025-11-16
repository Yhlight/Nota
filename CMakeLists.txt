cmake_minimum_required(VERSION 3.10)
project(Nota)

# Set the C++ standard to 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the main executable
add_executable(nota src/main.cpp)

# Add the tests subdirectory
add_subdirectory(tests)
