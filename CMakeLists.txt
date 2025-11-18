cmake_minimum_required(VERSION 3.10)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_library(nota_core src/Nota.h src/Nota.cpp)
target_include_directories(nota_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(nota app/main.cpp)
target_link_libraries(nota PRIVATE nota_core)

add_executable(nota_tests tests/main.cpp)
target_link_libraries(nota_tests PRIVATE nota_core gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
