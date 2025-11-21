#include "vendor/doctest.h"
#include "Parser.h"
#include "AST.h"
#include "Lexer.h"
#include <iostream>

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<T>& ptr) {
    if (ptr) {
        os << ptr.get();
    } else {
        os << "nullptr";
    }
    return os;
}


TEST_CASE("Parser parses a numeric literal") {
    nota::Lexer lexer("123;");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto exprStmt = std::dynamic_pointer_cast<nota::ExpressionStmt>(stmts[0]);
    REQUIRE(exprStmt);
    auto literal = std::dynamic_pointer_cast<nota::Literal>(exprStmt->expression);
    REQUIRE(literal);
    CHECK(std::get<int>(literal->value) == 123);
}

TEST_CASE("Parser parses a unary expression") {
    nota::Lexer lexer("-123;");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto exprStmt = std::dynamic_pointer_cast<nota::ExpressionStmt>(stmts[0]);
    REQUIRE(exprStmt);
    auto unary = std::dynamic_pointer_cast<nota::Unary>(exprStmt->expression);
    REQUIRE(unary);
    CHECK(unary->op.type == nota::TokenType::MINUS);
}

TEST_CASE("Parser parses a binary expression") {
    nota::Lexer lexer("1 + 2;");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto exprStmt = std::dynamic_pointer_cast<nota::ExpressionStmt>(stmts[0]);
    REQUIRE(exprStmt);
    auto binary = std::dynamic_pointer_cast<nota::Binary>(exprStmt->expression);
    REQUIRE(binary);
    CHECK(binary->op.type == nota::TokenType::PLUS);
}

TEST_CASE("Parser parses a grouping expression") {
    nota::Lexer lexer("(1 + 2);");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto exprStmt = std::dynamic_pointer_cast<nota::ExpressionStmt>(stmts[0]);
    REQUIRE(exprStmt);
    auto grouping = std::dynamic_pointer_cast<nota::Grouping>(exprStmt->expression);
    REQUIRE(grouping);
}

TEST_CASE("Parser parses a variable declaration") {
    nota::Lexer lexer("let a = 10;");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto varStmt = std::dynamic_pointer_cast<nota::VarStmt>(stmts[0]);
    REQUIRE(varStmt);
    CHECK(varStmt->name.lexeme == "a");
    auto literal = std::dynamic_pointer_cast<nota::Literal>(varStmt->initializer);
    REQUIRE(literal);
    CHECK(std::get<int>(literal->value) == 10);
}

TEST_CASE("Parser parses an if statement") {
    nota::Lexer lexer("if true 1; end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto ifStmt = std::dynamic_pointer_cast<nota::IfStmt>(stmts[0]);
    REQUIRE(ifStmt);
    auto condition = std::dynamic_pointer_cast<nota::Literal>(ifStmt->condition);
    REQUIRE(condition);
    auto thenBlock = std::dynamic_pointer_cast<nota::Block>(ifStmt->thenBranch);
    REQUIRE(thenBlock);
    REQUIRE(thenBlock->statements.size() == 1);
    auto thenBranch = std::dynamic_pointer_cast<nota::ExpressionStmt>(thenBlock->statements[0]);
    REQUIRE(thenBranch);
    REQUIRE(ifStmt->elseBranch == nullptr);
}

TEST_CASE("Parser parses an if-else statement") {
    nota::Lexer lexer("if true 1; else 2; end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto ifStmt = std::dynamic_pointer_cast<nota::IfStmt>(stmts[0]);
    REQUIRE(ifStmt);
    auto thenBlock = std::dynamic_pointer_cast<nota::Block>(ifStmt->thenBranch);
    REQUIRE(thenBlock);
    REQUIRE(thenBlock->statements.size() == 1);
    auto thenBranch = std::dynamic_pointer_cast<nota::ExpressionStmt>(thenBlock->statements[0]);
    REQUIRE(thenBranch);
    auto elseBranch = std::dynamic_pointer_cast<nota::ExpressionStmt>(ifStmt->elseBranch);
    REQUIRE(elseBranch);
}

TEST_CASE("Parser parses a while statement with a block") {
    nota::Lexer lexer("while true let a = 1; end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();

    REQUIRE(stmts.size() == 1);
    auto whileStmt = std::dynamic_pointer_cast<nota::WhileStmt>(stmts[0]);
    REQUIRE(whileStmt);
    auto condition = std::dynamic_pointer_cast<nota::Literal>(whileStmt->condition);
    REQUIRE(condition);
    auto body = std::dynamic_pointer_cast<nota::Block>(whileStmt->body);
    REQUIRE(body);
    REQUIRE(body->statements.size() == 1);
    auto varStmt = std::dynamic_pointer_cast<nota::VarStmt>(body->statements[0]);
    REQUIRE(varStmt);
}

TEST_CASE("Parser throws an error for an invalid expression") {
    nota::Lexer lexer("1 +;");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    CHECK_THROWS_AS(parser.parse(), nota::Parser::ParseError);
}
