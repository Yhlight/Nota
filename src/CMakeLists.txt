# 将此目录下的所有源文件编译成一个名为 NotaCore 的静态库
add_library(NotaCore)

# 明确地列出所有源文件
set(NOTA_SOURCES
    Lexer.cpp
    Parser.cpp
    SemanticAnalyzer.cpp
    SymbolTable.cpp
)

# 将源文件添加到库中
target_sources(NotaCore PRIVATE ${NOTA_SOURCES})

# 将 include 目录添加到 NotaCore 库的包含路径中
# 这使得其他目标（如测试）可以找到 NotaCore 的头文件
target_include_directories(NotaCore PUBLIC ../include)
