#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"

class PrintVisitor : public ASTVisitor {
    int indent = 0;
    void printIndent() {
        for(int i=0; i<indent; ++i) std::cout << "  ";
    }
public:
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

    void visit(ValueNode& node) override {
        std::cout << node.token.value;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: nota <file.nota>\n";
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << "\n";
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

        PrintVisitor printer;
        root->accept(printer);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
