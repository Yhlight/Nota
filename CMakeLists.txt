cmake_minimum_required(VERSION 3.10)
project(Nota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the GoogleTest subdirectory
add_subdirectory(third_party/googletest)

# Create a static library for the core application logic
add_library(nota_lib STATIC src/main.cpp)

# Create the main executable and link it against the library
add_executable(nota src/main.cpp)
target_link_libraries(nota PRIVATE nota_lib)

# Enable testing and add the tests subdirectory
enable_testing()
add_subdirectory(tests)
