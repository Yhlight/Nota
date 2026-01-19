#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "compiler.h"

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

    void visit(PackageNode& node) override {
        printIndent();
        std::cout << "Package: " << node.name << "\n";
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

    void visit(ConditionalNode& node) override {
        printIndent();
        std::cout << "If (";
        node.condition->accept(*this);
        std::cout << ") {\n";
        indent++;
        for (auto& child : node.thenBranch) {
            child->accept(*this);
        }
        indent--;
        printIndent();
        std::cout << "}";
        if (!node.elseBranch.empty()) {
            std::cout << " Else {\n";
            indent++;
            for (auto& child : node.elseBranch) {
                child->accept(*this);
            }
            indent--;
            printIndent();
            std::cout << "}";
        }
        std::cout << "\n";
    }

    void visit(StructDefinitionNode& node) override {
        printIndent();
        std::cout << "Struct " << node.name << " {\n";
        indent++;
        for (auto& field : node.fields) {
            printIndent();
            std::cout << field.type << " " << field.name;
            if (field.defaultValue) {
                std::cout << " = ";
                field.defaultValue->accept(*this);
            }
            std::cout << ";\n";
        }
        indent--;
        printIndent();
        std::cout << "}\n";
    }

    void visit(StructInstantiationNode& node) override {
        std::cout << node.structName << "(";
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            if (i > 0) std::cout << ", ";
            node.arguments[i]->accept(*this);
        }
        std::cout << ")";
    }

    void visit(ListNode& node) override {
        std::cout << "[";
        for (size_t i = 0; i < node.elements.size(); ++i) {
            if (i > 0) std::cout << ", ";
            node.elements[i]->accept(*this);
        }
        std::cout << "]";
    }

    void visit(DelegateNode& node) override {
        printIndent();
        std::cout << "Delegate [";
        for (size_t i = 0; i < node.events.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << node.events[i];
        }
        std::cout << "] for " << node.target << "\n";
    }

    void visit(ForNode& node) override {
        printIndent();
        std::cout << "For (" << node.iterator << " in " << node.listName << ") {\n";
        indent++;
        for (auto& child : node.body) {
            child->accept(*this);
        }
        indent--;
        printIndent();
        std::cout << "}\n";
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

    try {
        auto root = Compiler::parseFile(inputPath);

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
