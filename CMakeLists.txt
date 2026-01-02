cmake_minimum_required(VERSION 3.15)
project(Nota LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Nota src/main.cpp)

# Testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(NotaTests tests/test_main.cpp)
target_link_libraries(NotaTests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(NotaTests)
