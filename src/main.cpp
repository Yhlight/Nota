#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

class PrintVisitor : public ASTVisitor {
    int indent = 0;
    void printIndent() {
        for(int i=0; i<indent; ++i) std::cout << "  ";
    }
public:
    void visit(ProgramNode& node) override {
        std::cout << "Program {\n";
        indent++;
        for (auto& stmt : node.statements) {
            stmt->accept(*this);
        }
        indent--;
        std::cout << "}\n";
    }

    void visit(ImportNode& node) override {
        printIndent();
        std::cout << "Import: " << node.path;
        if (!node.alias.empty()) std::cout << " as " << node.alias;
        std::cout << "\n";
    }

    void visit(ComponentNode& node) override {
        printIndent();
        std::cout << "Component: " << node.type;
        if (!node.name.empty()) std::cout << " " << node.name;
        std::cout << " {\n";
        indent++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indent--;
        printIndent();
        std::cout << "}\n";
    }

    void visit(PropertyNode& node) override {
        printIndent();
        std::cout << "Property: " << node.name << " = ";
        if (node.value) node.value->accept(*this);
        else std::cout << "(null)";
        std::cout << "\n";
    }

    void visit(LiteralNode& node) override {
        std::cout << node.token.value;
    }

    void visit(ReferenceNode& node) override {
        std::cout << node.name;
    }

    void visit(BinaryExpressionNode& node) override {
        std::cout << "(";
        node.left->accept(*this);
        std::cout << " " << node.op.value << " ";
        node.right->accept(*this);
        std::cout << ")";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: nota <file.nota> [-o output.html]\n";
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath;

    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            outputPath = argv[++i];
        }
    }

    std::ifstream file(inputPath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << inputPath << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto root = parser.parse();

        if (outputPath.empty()) {
            // Default behavior: print AST
            PrintVisitor printer;
            root->accept(printer);
        } else {
            // Generate Code
            CodeGen codegen;
            std::string html = codegen.generate(*root);

            std::ofstream outFile(outputPath);
            if (!outFile.is_open()) {
                std::cerr << "Could not open output file: " << outputPath << "\n";
                return 1;
            }
            outFile << html;
            std::cout << "Generated " << outputPath << "\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
