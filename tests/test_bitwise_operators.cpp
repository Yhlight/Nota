#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

TEST_CASE("Bitwise operators") {
    SUBCASE("& operator") {
        std::string source = "let a = 5 & 3";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token a_token{nota::TokenType::IDENTIFIER, "a", {}, 1};
        auto a_val = env->get(a_token);
        CHECK(std::get<int>(a_val) == 1);
    }

    SUBCASE("| operator") {
        std::string source = "let a = 5 | 3";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token a_token{nota::TokenType::IDENTIFIER, "a", {}, 1};
        auto a_val = env->get(a_token);
        CHECK(std::get<int>(a_val) == 7);
    }

    SUBCASE("^ operator") {
        std::string source = "let a = 5 ^ 3";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token a_token{nota::TokenType::IDENTIFIER, "a", {}, 1};
        auto a_val = env->get(a_token);
        CHECK(std::get<int>(a_val) == 6);
    }

    SUBCASE("~ operator") {
        std::string source = "let a = ~5";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token a_token{nota::TokenType::IDENTIFIER, "a", {}, 1};
        auto a_val = env->get(a_token);
        CHECK(std::get<int>(a_val) == -6);
    }

    SUBCASE("<< operator") {
        std::string source = "let a = 5 << 1";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token a_token{nota::TokenType::IDENTIFIER, "a", {}, 1};
        auto a_val = env->get(a_token);
        CHECK(std::get<int>(a_val) == 10);
    }

    SUBCASE(">> operator") {
        std::string source = "let a = 5 >> 1";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token a_token{nota::TokenType::IDENTIFIER, "a", {}, 1};
        auto a_val = env->get(a_token);
        CHECK(std::get<int>(a_val) == 2);
    }
}
