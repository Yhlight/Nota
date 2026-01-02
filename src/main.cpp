#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalyzer.hpp"
#include "AST.hpp"

int main() {
    std::string source = R"(
        App {
            Row {
                spacing: 10
                Rect { width: 50; height: 50 }
            }
            App {
                spacing: 5 // Invalid
            }
        }
    )";

    // 1. Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // 2. Parser
    Parser parser(tokens);
    std::unique_ptr<Component> root = parser.parse();

    if (!root) {
        std::cerr << "Parsing failed." << std::endl;
        return 1;
    }

    // 3. Semantic Analyzer
    SemanticAnalyzer analyzer;
    analyzer.analyze(*root);

    const auto& errors = analyzer.getErrors();
    if (!errors.empty()) {
        std::cerr << "Semantic Errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "- " << error << std::endl;
        }
        return 1;
    }

    std::cout << "Compilation successful!" << std::endl;

    return 0;
}
