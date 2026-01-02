cmake_minimum_required(VERSION 3.15)
project(Nota)

# 设置 C++ 标准为 C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 src 子目录，其中包含核心库
add_subdirectory(src)

# 启用测试并添加 tests 子目录
enable_testing()
add_subdirectory(tests)
