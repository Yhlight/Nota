#include <gtest/gtest.h>
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include <any>

static std::any interpret(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);
    return interpreter.getLastValue();
}

TEST(InterpreterTest, EvaluatesArithmeticExpressions) {
    std::string source = "1 + 2\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 3);
}

TEST(InterpreterTest, HandlesFunctionCallsAndReturns) {
    std::string source = "func add(a, b)\nreturn a + b\nend\nadd(1, 2)\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 3);
}

TEST(InterpreterTest, HandlesVariableDeclarations) {
    std::string source = "let a = 1\nlet b = 2\nb\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 2);
}

TEST(InterpreterTest, HandlesIfStatements) {
    std::string source = "mut a = 1\nif 1 > 0\na = 2\nend\na\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 2);
}

TEST(InterpreterTest, HandlesIfElseStatements) {
    std::string source = "mut a = 1\nif 0 > 1\na = 2\nelse\na = 3\nend\na\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 3);
}

TEST(InterpreterTest, HandlesWhileStatements) {
    std::string source = "mut a = 0\nwhile a < 5\na = a + 1\nend\na\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 5);
}

TEST(InterpreterTest, HandlesArrayCreationAndIndexing) {
    std::string source = "let a = [1, \"two\", true]\na[1]\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<std::string>(value), "two");
}

TEST(InterpreterTest, HandlesStringConcatenation) {
    std::string source = "\"hello\" + \" \" + \"world\"\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<std::string>(value), "hello world");
}

TEST(InterpreterTest, HandlesPostfixIncrement) {
    std::string source = "mut a = 0\na++\na\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 1);
}

TEST(InterpreterTest, HandlesForStatements) {
    std::string source = "mut a = 0\nfor mut i = 0; i < 5; i++\na = a + 1\nend\na\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 5);
}

TEST(InterpreterTest, HandlesImplicitLineContinuation) {
    std::string source = "10 + \n 20\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 30);
}

TEST(InterpreterTest, HandlesDoWhileStatements) {
    std::string source = "mut a = 0\ndo\na = a + 1\nwhile a < 5\na\n";
    auto value = interpret(source);
    EXPECT_EQ(std::any_cast<int>(value), 5);
}
