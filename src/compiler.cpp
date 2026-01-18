#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

namespace Compiler {

std::shared_ptr<ProgramNode> parseFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    return parser.parse();
}

}
