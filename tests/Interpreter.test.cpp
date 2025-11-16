#include <gtest/gtest.h>
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include <sstream>
#include <iostream>

class InterpreterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Redirect cout and cerr to stringstreams
        sbuf_out = std::cout.rdbuf();
        std::cout.rdbuf(buffer_out.rdbuf());
        sbuf_err = std::cerr.rdbuf();
        std::cerr.rdbuf(buffer_err.rdbuf());
    }

    void TearDown() override {
        // Restore cout and cerr
        std::cout.rdbuf(sbuf_out);
        std::cerr.rdbuf(sbuf_err);
    }

    std::string getOutput() {
        return buffer_out.str();
    }

    std::string getErrorOutput() {
        return buffer_err.str();
    }

private:
    std::stringstream buffer_out;
    std::stringstream buffer_err;
    std::streambuf* sbuf_out;
    std::streambuf* sbuf_err;
};

TEST_F(InterpreterTest, PrintsCorrectValue) {
    std::string source = "print 2 * 3\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "6\n");
}

TEST_F(InterpreterTest, HandlesStringConcatenation) {
    std::string source = "print \"hello\" + \" world\"\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "hello world\n");
}

TEST_F(InterpreterTest, HandlesVariableDeclarationAndLookup) {
    std::string source = "var a = 10\n print a * 2\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "20\n");
}

TEST_F(InterpreterTest, HandlesIfStatement) {
    std::string source = "if true print 1 else print 2 end\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "1\n");
}

TEST_F(InterpreterTest, HandlesElseIfStatement) {
    std::string source = "if false print 1 else if true print 2 else print 3 end\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "2\n");
}


TEST_F(InterpreterTest, HandlesBlockScoping) {
    std::string source = "var a = 1\n if true var a = 2\n print a\n end\n print a\n";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);

    EXPECT_EQ(getOutput(), "2\n1\n");
}
