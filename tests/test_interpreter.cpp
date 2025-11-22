#include "vendor/doctest.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"
#include "VM.h"

TEST_CASE("Interpreter executes a while loop") {
    nota::Lexer lexer("mut a = 0\n while a < 5\n a = a + 1\n end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 5);
}

TEST_CASE("Interpreter handles correct scoping in a do-while loop") {
    std::string source = R"(
        mut a = 0
        mut total = 0
        do
            if a == 0
                mut b = 1
            end
            total = total + b
            b = b + 1
            a = a + 1
        while a < 2
    )";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto total_val = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "total", {}, 1});
    REQUIRE(std::holds_alternative<int>(total_val));
    CHECK(std::get<int>(total_val) == 3);

    auto a_val = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(a_val));
    CHECK(std::get<int>(a_val) == 2);
}

TEST_CASE("Interpreter executes a function call with return") {
    nota::Lexer lexer("fn add(a, b)\n return a + b\n end\n mut result = add(3, 4)\n");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "result", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 7);
}

TEST_CASE("Interpreter executes a function call") {
    nota::Lexer lexer("mut a = 0\n fn test(x)\n a = x\n end\n test(5)\n");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 5);
}

TEST_CASE("Interpreter executes an if statement") {
    nota::Lexer lexer("mut a = 0\n if true\n a = 1\n end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 1);
}

TEST_CASE("Interpreter executes a for loop") {
    nota::Lexer lexer("mut a = 0\n for mut i = 0; i < 5; i++\n a = a + i\n end");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 10);
}

TEST_CASE("Interpreter executes a do-while loop") {
    nota::Lexer lexer("mut a = 0\n do\n a = a + 1\n while a < 5\n");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto value = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<int>(value));
    CHECK(std::get<int>(value) == 5);
}
