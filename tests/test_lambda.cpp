#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

TEST_CASE("Lambda Functions") {
    SUBCASE("Single-expression lambda") {
        std::string source = R"(
            let add = (a, b) => a + b;
            let result = add(3, 4);
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 7);
    }

    SUBCASE("Block-body lambda") {
        std::string source = R"(
            let add = (a, b) => do
                return a + b;
            end;
            let result = add(5, 6);
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 11);
    }

    SUBCASE("Closure") {
        std::string source = R"(
            let x = 10;
            let my_lambda = () => x;
            let result = my_lambda();
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 10);
    }
}
