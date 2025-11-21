#include <iostream>
#include <fstream>
#include <sstream>
#include "lib/Lexer.h"
#include "lib/Parser.h"
#include "lib/Compiler.h"
#include "lib/VM.h"

void run(const std::string& source) {
    nota::VM vm;
    vm.interpret(source);
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
