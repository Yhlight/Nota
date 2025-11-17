#include "gtest/gtest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <vector>

void interpret(const std::string& source) {
    nota::Lexer lexer(source);
    nota::Parser parser(lexer);
    auto stmts = parser.parse();
    nota::Interpreter interpreter;
    interpreter.interpret(stmts);
}

TEST(InterpreterTest, VarDeclAndAssignment) {
    std::string source = "let a = 10\nlet b = a + 5\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, BlockScope) {
    std::string source = "let a = 10\nif a == 10\n let b = 20\nend\n";
    ASSERT_NO_THROW(interpret(source));
}

TEST(InterpreterTest, WhileLoop) {
    std::string source = "mut a = 0\nwhile a < 10\na = a + 1\nend\n";
    ASSERT_NO_THROW(interpret(source));
}
