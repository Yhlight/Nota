cmake_minimum_required(VERSION 3.20)
project(NotaCompiler CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- GoogleTest Integration ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows:
# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG main
# )
FetchContent_MakeAvailable(googletest)
# --- End of GoogleTest Integration ---


# --- CTest Integration ---
enable_testing()
# --- End of CTest Integration ---


add_executable(nota src/main.cpp)

target_include_directories(nota PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add the tests subdirectory
add_subdirectory(tests)
