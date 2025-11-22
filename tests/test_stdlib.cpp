#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>
#include <iostream>
#include <sstream>

// Helper to redirect stdout
struct CoutRedirect {
    CoutRedirect(std::streambuf* new_buffer)
        : old(std::cout.rdbuf(new_buffer))
    { }

    ~CoutRedirect() {
        std::cout.rdbuf(old);
    }

private:
    std::streambuf* old;
};

TEST_CASE("Standard Library Functions") {
    SUBCASE("print function") {
        std::string source = R"(
            print("Hello, world!")
            print(123)
            print(true)
        )";

        std::stringstream buffer;
        CoutRedirect redirect(buffer.rdbuf());

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        interpreter->interpret(statements);

        CHECK(buffer.str() == "Hello, world!\n123\ntrue\n");
    }

    SUBCASE("clock function") {
        std::string source = "let a = clock()";

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
        CHECK(std::holds_alternative<double>(a_val));
        CHECK(std::get<double>(a_val) > 0.0);
    }
}
