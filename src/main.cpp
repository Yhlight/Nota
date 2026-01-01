#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

int main(int argc, char* argv[]) {
    // 检查命令行参数
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.nota>" << std::endl;
        return 1;
    }

    // 读取输入文件
    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string sourceCode = buffer.str();

    // --- 编译流程 ---
    // 1. 词法分析
    Lexer lexer(sourceCode);

    // 2. 语法分析
    Parser parser(lexer);
    std::shared_ptr<ProgramNode> ast = parser.ParseProgram();

    // 检查解析错误
    if (!parser.Errors().empty()) {
        std::cerr << "Parser errors:\n";
        for (const auto& error : parser.Errors()) {
            std::cerr << "- " << error << "\n";
        }
        return 1;
    }

    // 3. 代码生成
    Generator generator;
    std::string output = generator.Generate(*ast);

    // 打印结果到标准输出
    std::cout << output << std::endl;

    return 0;
}
