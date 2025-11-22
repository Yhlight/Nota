#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

// --- Native functions for testing ---

int native_add(int a, int b) {
    return a + b;
}

std::string native_concat(std::string a, std::string b) {
    return a + b;
}

void native_void_func() {
    // Does nothing
}

TEST_CASE("FFI") {
    SUBCASE("Simple FFI call") {
        std::string source = R"(
            let result = native_add(10, 20)
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        interpreter->registerNative("native_add", native_add);

        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        CHECK(std::get<int>(result_val) == 30);
    }

    SUBCASE("String FFI call") {
        std::string source = R"(
            let result = native_concat("hello", " world")
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        interpreter->registerNative("native_concat", native_concat);

        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        interpreter->interpret(statements);

        auto env = interpreter->getEnvironment();
        nota::Token result_token{nota::TokenType::IDENTIFIER, "result", {}, 1};
        auto result_val = env->get(result_token);
        auto str_obj = dynamic_cast<nota::NotaString*>(std::get<nota::Object*>(result_val));
        CHECK(str_obj->value == "hello world");
    }

    SUBCASE("Void FFI call") {
        std::string source = R"(
            native_void_func()
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        interpreter->registerNative("native_void_func", native_void_func);

        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        // Should not throw an exception
        interpreter->interpret(statements);
    }

    SUBCASE("FFI type error") {
        std::string source = R"(
            let result = native_add("a", "b")
        )";

        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);
        interpreter->registerNative("native_add", native_add);

        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        CHECK_THROWS_AS(interpreter->interpret(statements), const nota::Interpreter::RuntimeError&);
    }
}
