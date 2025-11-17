#include "TestHelper.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "ModuleManager.h"
#include <unistd.h>
#include <fstream>
#include <sstream>

static nota::ModuleManager module_manager;

void interpret(const std::string& source) {
    chdir("build/tests");
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::Interpreter interpreter(module_manager);
    interpreter.interpret(stmts, "test.nota");
}

void interpret_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::Interpreter interpreter(module_manager);
    interpreter.interpret(stmts, path);
}
