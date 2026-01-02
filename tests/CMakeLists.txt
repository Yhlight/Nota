# 添加一个新的可执行文件目标，用于运行测试
add_executable(NotaTests)

# 明确地列出所有测试源文件
set(TEST_SOURCES
    LexerTests.cpp
    ParserTests.cpp
    SemanticAnalyzerTests.cpp
)

# 将测试源文件添加到可执行文件中
target_sources(NotaTests PRIVATE ${TEST_SOURCES})

# 包含 FetchContent 模块，以便从 Git 仓库中获取 GoogleTest
include(FetchContent)

# 声明 GoogleTest 的内容
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG    release-1.11.0
)

# 使 GoogleTest 可用
FetchContent_MakeAvailable(googletest)

# 包含 GoogleTest 模块以使用 gtest_discover_tests
include(GoogleTest)

# 将 NotaTests 可执行文件与 GoogleTest 和我们自己的 NotaCore 库链接
target_link_libraries(NotaTests PRIVATE gtest_main NotaCore)

# 发现 NotaTests 可执行文件中的所有测试
gtest_discover_tests(NotaTests)
