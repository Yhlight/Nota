#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

TEST_CASE("Logical operators") {
    SUBCASE("AND operator") {
        std::string source = "let a = true && false";
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
        CHECK(std::get<bool>(a_val) == false);
    }

    SUBCASE("OR operator") {
        std::string source = "let a = true || false";
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
        CHECK(std::get<bool>(a_val) == true);
    }

    SUBCASE("Short-circuiting AND") {
        std::string source = R"(
            let a = false
            let b = true
            let result = a && (b = false)
        )";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token b_token{nota::TokenType::IDENTIFIER, "b", {}, 1};
        auto b_val = env->get(b_token);
        CHECK(std::get<bool>(b_val) == true); // b should not have been assigned to false
    }

    SUBCASE("Short-circuiting OR") {
        std::string source = R"(
            let a = true
            let b = true
            let result = a || (b = false)
        )";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token b_token{nota::TokenType::IDENTIFIER, "b", {}, 1};
        auto b_val = env->get(b_token);
        CHECK(std::get<bool>(b_val) == true); // b should not have been assigned to false
    }

    SUBCASE("Assignment expression returns value") {
        std::string source = "mut b; let a = (b = 5)";
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
        CHECK(std::get<int>(a_val) == 5);
        nota::Token b_token{nota::TokenType::IDENTIFIER, "b", {}, 1};
        auto b_val = env->get(b_token);
        CHECK(std::get<int>(b_val) == 5);
    }
}
