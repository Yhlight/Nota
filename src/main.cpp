#include <iostream>
#include <fstream>
#include <sstream>
#include "lib/Lexer.h"
#include "lib/Parser.h"
#include "lib/Compiler.h"
#include "lib/VM.h"

void run(const std::string& source) {
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();

    if (tokens.empty()) {
        return;
    }

    nota::Parser parser(tokens);
    auto statements = parser.parse();

    if (!statements.empty()) {
        nota::Compiler compiler;
        nota::Chunk chunk = compiler.compile(statements);

        nota::VM vm;
        vm.interpret(&chunk);
    }
}

void runFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        return;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    run(buffer.str());
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        runFile(argv[1]);
    } else {
        std::cout << "Usage: nota [script]" << std::endl;
    }
    return 0;
}
