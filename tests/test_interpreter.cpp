#include "vendor/doctest.h"
#include "vendor/doctest.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"

TEST_CASE("Interpreter executes a while loop") {
    nota::Lexer lexer("mut a = 0; while a < 5 a = a + 1; end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::Interpreter interpreter;
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment().get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 5);
}

TEST_CASE("Interpreter executes an if statement") {
    nota::Lexer lexer("mut a = 0; if true a = 1; end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::Interpreter interpreter;
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment().get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 1);
}
