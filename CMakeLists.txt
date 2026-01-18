cmake_minimum_required(VERSION 3.14)
project(nota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

include_directories(include)

# Source directory
add_subdirectory(src)

# Tests directory
enable_testing()
add_subdirectory(tests)
