#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

// 辅助函数，用于从文件读取内容
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// --- 快照测试 ---
TEST(CompilerSnapshotTest, BasicApp) {
    // 1. 读取 Nota 源文件
    // 路径是相对于 build/tests 目录下的可执行文件
    std::string sourceCode = readFile("../../tests/sample.nota");

    // 2. 运行编译流程
    Lexer lexer(sourceCode);
    Parser parser(lexer);
    auto ast = parser.ParseProgram();
    ASSERT_TRUE(parser.Errors().empty()); // 确保没有解析错误

    Generator generator;
    std::string actualOutput = generator.Generate(*ast);

    // 3. 定义预期的“快照”输出
    // 使用 C++11 的原始字符串字面量 R"(...)" 来避免转义
    std::string expectedOutput = R"(<!DOCTYPE html>
<html>
<head>
  <style>
.nota-0 {
  box-sizing: border-box;
  overflow: hidden;
  margin: 0;
  padding: 0;
  width: 800px;
  height: 600px;
  background-color: #f0f0f0;
}

.nota-1 {
  box-sizing: border-box;
  overflow: hidden;
  margin: 0;
  padding: 0;
  display: flex;
  flex-direction: row;
  width: 100%;
  height: 50px;
  gap: 10px;
  background-color: blue;
}

.nota-2 {
  box-sizing: border-box;
  overflow: hidden;
  margin: 0;
  padding: 0;
  display: block;
  width: 50px;
  height: 50px;
  background-color: red;
}

  </style>
</head>
<body class="nota-app nota-0">
<div class="nota-row nota-1">
<div class="nota-rect nota-2">
</div>
</div>
</body>
</html>)";

    // 4. 比较实际输出和预期快照
    EXPECT_EQ(actualOutput, expectedOutput);
}
