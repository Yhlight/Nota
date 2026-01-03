# Nota 编译器开发者文档

本文档为有兴趣为 Nota 编译器做出贡献的开发者提供了高级概述。

## 项目结构

Nota 编译器的源代码被组织成以下目录：

*   `src`: 包含编译器本身的源代码。
    *   `analyzer`: 语义分析器，负责验证 Nota 代码的正确性。
    *   `ast`: 抽象语法树 (AST) 节点的定义。
    *   `codegen`: 代码生成器，负责将 AST 转换为 HTML 和 CSS。
    *   `core`: 编译器使用的核心数据结构，例如 `Token` 和 `Symbol`。
    *   `lexer`: 词法分析器，负责将 Nota 源代码分解为标记流。
    *   `parser`: 解析器，负责将标记流构建成 AST。
*   `tests`: 包含编译器的单元测试。
*   `benchmarks`: 包含用于测量编译器性能的基准测试。

## 编译器架构

Nota 编译器遵循经典的编译器设计，具有以下阶段：

1.  **词法分析**: `Lexer` 将 Nota 源代码作为输入，并将其分解为一系列标记。
2.  **解析**: `Parser` 接收标记流并构建一个表示代码结构的抽象语法树 (AST)。
3.  **语义分析**: `SemanticAnalyzer` 遍历 AST 并验证其是否符合 Nota 语言的规则。这包括类型检查、作用域解析和属性验证。
4.  **代码生成**: `CodeGenerator` 接收经过验证的 AST 并生成等效的 HTML 和 CSS 代码。

## 构建和测试

Nota 编译器是使用 CMake 构建的。要构建编译器，请运行以下命令：

```bash
mkdir build
cd build
cmake ..
make
```

这将把 `Nota` 可执行文件生成在 `build` 目录中。

要运行单元测试，请在 `build` 目录中运行以下命令：

```bash
ctest
```

要运行基准测试，请运行以下脚本：

```bash
./benchmarks/run_benchmark.sh
```
