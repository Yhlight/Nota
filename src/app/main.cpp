#include "nota.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

static void run(const std::string& source) {
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> statements = parser.parse();

    if (parser.hadError()) {
        return;
    }

    interpreter.interpret(statements);
}

static void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Could not open file " << path << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

static void runPrompt() {
    std::cout << "> ";
    std::string line;
    while (std::getline(std::cin, line)) {
        run(line);
        std::cout << "> ";
    }
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cout << "Usage: nota [script]" << std::endl;
        return 64;
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        runPrompt();
    }

    return 0;
}
