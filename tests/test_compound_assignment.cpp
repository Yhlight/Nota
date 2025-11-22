#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

TEST_CASE("Compound assignment operators") {
    SUBCASE("+= operator") {
        std::string source = "mut a = 1; a += 2";
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
        CHECK(std::get<int>(a_val) == 3);
    }

    SUBCASE("-= operator") {
        std::string source = "mut a = 5; a -= 2";
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
        CHECK(std::get<int>(a_val) == 3);
    }

    SUBCASE("*= operator") {
        std::string source = "mut a = 3; a *= 4";
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
        CHECK(std::get<int>(a_val) == 12);
    }

    SUBCASE("/= operator") {
        std::string source = "mut a = 12; a /= 4";
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
        CHECK(std::get<int>(a_val) == 3);
    }

    SUBCASE("%= operator") {
        std::string source = "mut a = 13; a %= 5";
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
        CHECK(std::get<int>(a_val) == 3);
    }
}
