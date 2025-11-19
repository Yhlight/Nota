#include <gtest/gtest.h>
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"

TEST(InterpreterTest, EvaluatesArithmeticExpressions) {
    std::string source = "let a = 1 + 2\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);

    auto globals = interpreter.getGlobals();
    auto value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    EXPECT_EQ(std::get<int>(value), 3);
}

TEST(InterpreterTest, HandlesVariableDeclarations) {
    std::string source = "let a = 1\n mut b = 2\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);

    auto globals = interpreter.getGlobals();
    auto a_value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    auto b_value = globals->get(Token{TokenType::IDENTIFIER, "b", std::monostate{}, 2});
    EXPECT_EQ(std::get<int>(a_value), 1);
    EXPECT_EQ(std::get<int>(b_value), 2);
}

TEST(InterpreterTest, HandlesIfStatements) {
    std::string source = "mut a = 1\nif 1 > 0\na = 2\nend\n";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);
    auto globals = interpreter.getGlobals();
    auto value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    EXPECT_EQ(std::get<int>(value), 2);
}

TEST(InterpreterTest, HandlesIfElseStatements) {
    std::string source = "mut a = 1\nif 0 > 1\na = 2\nelse\na = 3\nend\n";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);
    auto globals = interpreter.getGlobals();
    auto value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    EXPECT_EQ(std::get<int>(value), 3);
}

TEST(InterpreterTest, HandlesWhileStatements) {
    std::string source = "mut a = 0\nwhile a < 5\na = a + 1\nend\n";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);
    auto globals = interpreter.getGlobals();
    auto value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    EXPECT_EQ(std::get<int>(value), 5);
}

TEST(InterpreterTest, HandlesPostfixIncrement) {
    std::string source = "mut a = 0\na++\n";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);
    auto globals = interpreter.getGlobals();
    auto value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    EXPECT_EQ(std::get<int>(value), 1);
}

TEST(InterpreterTest, HandlesForStatements) {
    std::string source = "mut a = 0\nfor mut i = 0; i < 5; i++\na = a + 1\nend\n";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(statements);
    auto globals = interpreter.getGlobals();
    auto value = globals->get(Token{TokenType::IDENTIFIER, "a", std::monostate{}, 1});
    EXPECT_EQ(std::get<int>(value), 5);
}
