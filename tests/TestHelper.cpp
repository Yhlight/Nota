#include "TestHelper.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

void interpret(const std::string& source) {
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::Interpreter interpreter;
    interpreter.interpret(stmts);
}
