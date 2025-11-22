#include "vendor/doctest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "test_helpers.h"
#include "VM.h"
#include <memory>

TEST_CASE("Modules") {
    SUBCASE("Import and access a function") {
        createTestFile("tests/test_files/module_a.nota", R"(
            fn add(a, b)
                return a + b
            end
        )");

        std::string source = R"(
            import "tests/test_files/module_a.nota"
            let result = module_a::add(1, 2)
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        interpreter.interpret(statements);

        auto env = interpreter.getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 3);
    }

    SUBCASE("Import with alias") {
        createTestFile("tests/test_files/module_b.nota", R"(
            let my_var = 123
        )");

        std::string source = R"(
            import "tests/test_files/module_b.nota" as b
            let result = b::my_var
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        interpreter.interpret(statements);

        auto env = interpreter.getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 123);
    }

    SUBCASE("Import a class") {
        createTestFile("tests/test_files/module_c.nota", R"(
            class MyClass
                fn get_val()
                    return 456
                end
            end
        )");

        std::string source = R"(
            import "tests/test_files/module_c.nota"
            let instance = module_c::MyClass()
            let result = instance.get_val()
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        interpreter.interpret(statements);

        auto env = interpreter.getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 456);
    }

    SUBCASE("Package statement") {
        std::string source = R"(
            package my_package
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        interpreter.interpret(statements);

        CHECK(interpreter.getPackageName() == "my_package");
    }
}
