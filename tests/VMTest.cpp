#include "gtest/gtest.h"
#include "VM.h"
#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"

using namespace nota;

TEST(VMTest, SimpleExpression) {
    std::string source = "1 + 2\n";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    Compiler compiler;
    Chunk chunk = compiler.compile(stmts);
    VM vm;
    InterpretResult result = vm.interpret(chunk);
    ASSERT_EQ(result, InterpretResult::Ok);
}

TEST(VMTest, UnaryMinus) {
    std::string source = "-1\n";
    Lexer lexer(source);
    Parser parser(lexer);
    auto stmts = parser.parse();
    Compiler compiler;
    Chunk chunk = compiler.compile(stmts);
    VM vm;
    InterpretResult result = vm.interpret(chunk);
    ASSERT_EQ(result, InterpretResult::Ok);
}
