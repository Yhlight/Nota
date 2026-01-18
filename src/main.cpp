#include <iostream>
#include <vector>
#include <string>
#include "include/Lexer.hpp"
#include "include/Parser.hpp"
#include "include/AST/AstNodes.hpp"

// Forward declaration for the recursive print function
void printAst(const ComponentNode& node, int indent = 0);

// Helper to print the AST
void printAst(const ComponentNode& node, int indent) {
    std::string indentation(indent * 2, ' ');
    std::cout << indentation << "Component: " << node.type.lexeme << std::endl;

    for (const auto& prop : node.properties) {
        std::cout << indentation << "  Property: " << prop->key.lexeme << " = " << prop->value.lexeme << std::endl;
    }

    for (const auto& child : node.children) {
        printAst(*child, indent + 1);
    }
}

int main(int argc, char* argv[]) {
    std::string source = R"(
        // Nota Parser Demonstration
        App {
            width: 100

            Rect {
                color: "blue"
                height: 50
            }
        }
    )";

    std::cout << "--- Parsing Nota Source ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "--------------------------" << std::endl;
    std::cout << "--- Abstract Syntax Tree ---" << std::endl;

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    try {
        std::unique_ptr<ComponentNode> root = parser.parse();
        if (root) {
            printAst(*root);
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
