cmake_minimum_required(VERSION 3.10)
project(Nota)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Core Library
file(GLOB LIBNOTA_SOURCES "src/lib/*.cpp" "src/lib/*.h")
add_library(libnota ${LIBNOTA_SOURCES})
target_include_directories(libnota PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/lib)

# Main Executable
add_executable(nota src/app/main.cpp)
target_link_libraries(nota libnota)

# Tests
file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(nota_tests ${TEST_SOURCES})
target_link_libraries(nota_tests libnota GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(nota_tests)
