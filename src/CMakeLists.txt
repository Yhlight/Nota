# 将此目录下的所有源文件编译成一个名为 NotaCore 的静态库
add_library(NotaCore)

# 查找当前目录下的所有 .cpp 和 .hpp 文件
file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

# 将源文件添加到库中
target_sources(NotaCore PRIVATE ${SOURCES})

# 将 include 目录添加到 NotaCore 库的包含路径中
# 这使得其他目标（如测试）可以找到 NotaCore 的头文件
target_include_directories(NotaCore PUBLIC ../include)
