#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalyzer.hpp"
#include "CodeGenerator.hpp"
#include "AST.hpp"

int main() {
    std::string source = R"(
        App {
            width: "100%"
            height: "100%"
            color: "#f0f0f0"

            Row {
                spacing: 20
                Rect { width: 40; height: 40; radius: 20; color: "white" }
                Text { text: "Nota Dashboard"; color: "white" }
            }
        }
    )";

    // 1. Lexer
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // 2. Parser
    Parser parser(tokens);
    std::unique_ptr<Component> root = nullptr;
    try {
        root = parser.parse();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing error: " << e.what() << std::endl;
        return 1;
    }

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

    // 4. Code Generator
    CodeGenerator generator;
    generator.generate(*root);

    std::cout << "<!-- Generated HTML -->" << std::endl;
    std::cout << generator.getHTML() << std::endl;
    std::cout << std::endl;
    std::cout << "/* Generated CSS */" << std::endl;
    std::cout << "<style>" << std::endl;
    std::cout << generator.getCSS() << std::endl;
    std::cout << "</style>" << std::endl;

    return 0;
}
