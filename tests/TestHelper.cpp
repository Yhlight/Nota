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

nota::ModuleManager &get_module_manager() {
    return module_manager;
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

void interpret_with_search_path(const std::string &source, const std::string &search_path) {
    module_manager.add_search_path(search_path);
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::Interpreter interpreter(module_manager);
    interpreter.interpret(stmts, "test.nota");
}
