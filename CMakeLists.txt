cmake_minimum_required(VERSION 3.10)
project(Nota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the GoogleTest subdirectory
add_subdirectory(third_party/googletest)

# Add source files and define the nota_lib library
add_subdirectory(src)

# Create the main executable and link it against the library
add_executable(nota src/main.cpp)
target_link_libraries(nota PRIVATE nota_lib)

# Enable testing and add the tests subdirectory
enable_testing()
add_subdirectory(tests)
