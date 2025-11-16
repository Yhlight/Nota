cmake_minimum_required(VERSION 3.10)
project(Nota VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)

# Add GoogleTest
add_subdirectory(third_party/googletest)

add_subdirectory(tests)

enable_testing()
