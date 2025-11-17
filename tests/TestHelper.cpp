#include "TestHelper.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "ModuleManager.h"
#include <unistd.h>

void interpret(const std::string& source) {
    chdir("build/tests");
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::ModuleManager module_manager;
    nota::Interpreter interpreter(module_manager);
    interpreter.interpret(stmts);
}
