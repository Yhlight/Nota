# CMakeLists.txt for the tests directory

# 创建测试可执行文件
# 我们需要包含编译器的源文件，以便在测试中直接调用它们
add_executable(nota_compiler_tests
    CompilerTest.cpp
    ../src/Lexer.cpp
    ../src/Parser.cpp
    ../src/Generator.cpp
)

# 链接 GoogleTest 库
target_link_libraries(nota_compiler_tests PRIVATE gtest gtest_main)

# 将 src 目录添加到 include 路径，以便测试代码能找到头文件
target_include_directories(nota_compiler_tests PRIVATE ../src)

# 将此可执行文件添加为一个测试
include(GoogleTest)
gtest_add_tests(TARGET nota_compiler_tests)
