cmake_minimum_required(VERSION 3.20)
project(NotaCompiler CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nota src/main.cpp)

target_include_directories(nota PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
