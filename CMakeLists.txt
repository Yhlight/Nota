# CMake 最低版本要求
cmake_minimum_required(VERSION 3.15)

# 项目信息
project(NotaCompiler LANGUAGES CXX)

# 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 将当前目录添加到 include 路径，方便头文件引用
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# --- GoogleTest 集成 ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# 对于 Windows：防止覆盖父项目的编译器/链接器设置
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 启用测试
enable_testing()

# 添加子目录
add_subdirectory(src)
add_subdirectory(tests)

# 为测试目标设置 include 目录和链接库
# 这是一个更健壮的做法，确保测试目标能找到 gtest
# 我们将在 tests/CMakeLists.txt 中定义 nota_compiler_tests
# 此处暂时注释，后续在 tests/CMakeLists.txt 中处理
# target_include_directories(nota_compiler_tests PRIVATE ${googletest_SOURCE_DIR}/googletest/include)
# target_link_libraries(nota_compiler_tests PRIVATE gtest gtest_main)
