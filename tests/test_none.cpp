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

TEST_CASE("None keyword") {
    SUBCASE("Assigning none to a variable") {
        std::string source = "let a = none";
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
        CHECK(std::holds_alternative<std::monostate>(a_val));
    }

    SUBCASE("Printing none") {
        std::string source = "print(none)";
        std::stringstream buffer;
        CoutRedirect redirect(buffer.rdbuf());

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();
        interpreter->interpret(statements);

        CHECK(buffer.str() == "none\n");
    }

    SUBCASE("Printing functions, classes, and instances") {
        std::string source = R"(
            fn my_func() end
            class MyClass end
            let instance = MyClass()
            print(my_func, MyClass, instance)
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

        CHECK(buffer.str() == "<fn my_func> MyClass MyClass instance\n");
    }
}
