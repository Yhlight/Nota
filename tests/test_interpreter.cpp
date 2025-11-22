#include "vendor/doctest.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"
#include "VM.h"

TEST_CASE("Interpreter executes a while loop") {
    nota::Lexer lexer("mut a = 0; while a < 5 a = a + 1; end");
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

TEST_CASE("Interpreter handles array creation, access, and assignment") {
    std::string source = R"(
        let a = [1, 2, 3];
        let b = a[0];
        a[1] = 5;
    )";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    interpreter.interpret(stmts);

    auto b_val = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "b", {}, 1});
    REQUIRE(std::holds_alternative<int>(b_val));
    CHECK(std::get<int>(b_val) == 1);

    auto a_val = interpreter.getEnvironment()->get({nota::TokenType::IDENTIFIER, "a", {}, 1});
    REQUIRE(std::holds_alternative<nota::Object*>(a_val));
    auto array = dynamic_cast<nota::NotaArray*>(std::get<nota::Object*>(a_val));
    REQUIRE(array);
    REQUIRE(array->elements.size() == 3);
    REQUIRE(std::holds_alternative<int>(array->elements[1]));
    CHECK(std::get<int>(array->elements[1]) == 5);
}

TEST_CASE("Interpreter throws error for out-of-bounds array access") {
    std::string source = R"(
        let a = [1, 2, 3];
        a[3];
    )";
    nota::Lexer lexer(source);
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::vector<std::shared_ptr<nota::Stmt>> stmts = parser.parse();
    nota::VM vm;
    nota::Interpreter interpreter(vm);
    CHECK_THROWS_AS(interpreter.interpret(stmts), nota::Interpreter::RuntimeError);
}

TEST_CASE("Interpreter handles correct scoping in a do-while loop") {
    std::string source = R"(
        mut a = 0;
        mut total = 0;
        do
            if a == 0
                mut b = 1;
            end
            total = total + b;
            b = b + 1;
            a = a + 1;
        while a < 2;
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
    nota::Lexer lexer("fn add(a, b) return a + b; end mut result = add(3, 4);");
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
    nota::Lexer lexer("mut a = 0; fn test(x) a = x; end test(5);");
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
    nota::Lexer lexer("mut a = 0; if true a = 1; end");
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
    nota::Lexer lexer("mut a = 0; for (mut i = 0; i < 5; i++) a = a + i; end");
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
    nota::Lexer lexer("mut a = 0; do a = a + 1; while a < 5;");
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
