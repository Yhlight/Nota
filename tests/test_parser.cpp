#include "vendor/doctest.h"
#include "../src/lib/Lexer.h"
#include "../src/lib/Parser.h"
#include "../src/lib/Expr.h"
#include "../src/lib/Stmt.h"

TEST_CASE("testing the parser with a variable declaration") {
    std::string source = "let a = 1;";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    CHECK(statements.size() == 1);
    auto varDecl = dynamic_cast<nota::VarDeclStmt*>(statements[0].get());
    CHECK(varDecl != nullptr);
    CHECK(varDecl->name.lexeme == "a");

    auto literal = dynamic_cast<nota::Literal*>(varDecl->initializer.get());
    CHECK(literal != nullptr);
    CHECK(std::any_cast<double>(literal->value) == 1.0);
}

TEST_CASE("testing the parser with a function declaration") {
    std::string source = "fn hello() end";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    CHECK(statements.size() == 1);
    auto funcDecl = dynamic_cast<nota::FunctionStmt*>(statements[0].get());
    CHECK(funcDecl != nullptr);
    CHECK(funcDecl->name.lexeme == "hello");
    CHECK(funcDecl->params.empty());
    CHECK(funcDecl->body.empty());
}

TEST_CASE("testing the parser with a function declaration with parameters and body") {
    std::string source = "fn add(a, b) let c = a + b; return c; end";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    CHECK(statements.size() == 1);
    auto funcDecl = dynamic_cast<nota::FunctionStmt*>(statements[0].get());
    CHECK(funcDecl != nullptr);
    CHECK(funcDecl->name.lexeme == "add");
    CHECK(funcDecl->params.size() == 2);
    CHECK(funcDecl->params[0].lexeme == "a");
    CHECK(funcDecl->params[1].lexeme == "b");
    CHECK(funcDecl->body.size() == 2);
}

TEST_CASE("testing the parser with an assignment") {
    std::string source = "a = 1;";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();

    CHECK(statements.size() == 1);
    auto exprStmt = dynamic_cast<nota::ExpressionStmt*>(statements[0].get());
    CHECK(exprStmt != nullptr);

    auto assignExpr = dynamic_cast<nota::AssignExpr*>(exprStmt->expression.get());
    CHECK(assignExpr != nullptr);
    CHECK(assignExpr->name.lexeme == "a");

    auto literal = dynamic_cast<nota::Literal*>(assignExpr->value.get());
    CHECK(literal != nullptr);
    CHECK(std::any_cast<double>(literal->value) == 1.0);
}
