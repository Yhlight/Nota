#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

TEST_CASE("For-each loop") {
    SUBCASE("Iterating over an array") {
        std::string source = R"(
            let a = [1, 2, 3]
            mut sum = 0
            for let i : a
                sum = sum + i
            end
        )";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token sum_token{nota::TokenType::IDENTIFIER, "sum", {}, 1};
        auto sum_val = env->get(sum_token);
        CHECK(std::get<int>(sum_val) == 6);
    }

    SUBCASE("Scoping of loop variable") {
        std::string source = R"(
            let a = [1]
            for let i : a
                // i is in scope here
            end
            // i should not be in scope here
        )";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token i_token{nota::TokenType::IDENTIFIER, "i", {}, 1};
        CHECK_THROWS_AS(env->get(i_token), nota::Interpreter::RuntimeError);
    }

    SUBCASE("Iterating over a non-iterable") {
        std::string source = "for let i : 123 end";
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        CHECK_THROWS_AS(interpreter->interpret(statements), nota::Interpreter::RuntimeError);
    }
}
