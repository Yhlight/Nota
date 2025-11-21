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

TEST_CASE("Parser throws an error for an invalid expression") {
    nota::Lexer lexer("1 +;");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    CHECK_THROWS_AS(parser.parse(), nota::Parser::ParseError);
}
