#include <gtest/gtest.h>
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include <sstream>
#include <iostream>

class InterpreterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Redirect cout to a stringstream
        sbuf = std::cout.rdbuf();
        std::cout.rdbuf(buffer.rdbuf());
    }

    void TearDown() override {
        // Restore cout
        std::cout.rdbuf(sbuf);
    }

    std::string getOutput() {
        return buffer.str();
    }

private:
    std::stringstream buffer;
    std::streambuf* sbuf;
};

TEST_F(InterpreterTest, PrintsCorrectValue) {
    std::string source = "print 2 * 3;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "6\n");
}

TEST_F(InterpreterTest, HandlesStringConcatenation) {
    std::string source = "print \"hello\" + \" world\";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "hello world\n");
}

TEST_F(InterpreterTest, HandlesVariableDeclarationAndLookup) {
    std::string source = "var a = 10; print a * 2;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "20\n");
}
