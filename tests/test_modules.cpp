#include "vendor/doctest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include <memory>
#include <fstream>
#include <filesystem>

// Helper function to create a test file
void createTestFile(const std::string& path, const std::string& content) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path);
    file << content;
}

TEST_CASE("Modules") {
    SUBCASE("Import and access a function") {
        createTestFile("tests/test_files/module_a.nota", R"(
            fn add(a, b)
                return a + b;
            end
        )");

        std::string source = R"(
            import "tests/test_files/module_a.nota";
            let result = module_a::add(1, 2);
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        auto interpreter = std::make_shared<nota::Interpreter>();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 3);
    }

    SUBCASE("Import with alias") {
        createTestFile("tests/test_files/module_b.nota", R"(
            let my_var = 123;
        )");

        std::string source = R"(
            import "tests/test_files/module_b.nota" as b;
            let result = b::my_var;
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        auto interpreter = std::make_shared<nota::Interpreter>();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 123);
    }

    SUBCASE("Import a class") {
        createTestFile("tests/test_files/module_c.nota", R"(
            class MyClass
                fn get_val()
                    return 456;
                end
            end
        )");

        std::string source = R"(
            import "tests/test_files/module_c.nota";
            let instance = module_c::MyClass();
            let result = instance.get_val();
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        auto interpreter = std::make_shared<nota::Interpreter>();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 456);
    }
}
