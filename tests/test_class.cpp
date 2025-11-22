#include "vendor/doctest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "NotaObjects.h"
#include "test_helpers.h"
#include "VM.h"
#include <memory>

TEST_CASE("Classes") {
    SUBCASE("Declare and instantiate a class") {
        std::string source = R"(
            class MyClass
            end

            mut instance = MyClass()
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        interpreter.interpret(statements);

        auto env = interpreter.getEnvironment();
        nota::Token instance_token{nota::TokenType::IDENTIFIER, "instance", {}, 1};
        auto val = env->get(instance_token);

        CHECK(std::holds_alternative<nota::Object*>(val));
        CHECK(dynamic_cast<nota::NotaInstance*>(std::get<nota::Object*>(val)) != nullptr);
    }

    SUBCASE("Get and set properties") {
        std::string source = R"(
            class MyClass
            end

            mut instance = MyClass()
            instance.foo = 123
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        interpreter.interpret(statements);

        auto env = interpreter.getEnvironment();
        nota::Token instance_token{nota::TokenType::IDENTIFIER, "instance", {}, 1};
        auto val = env->get(instance_token);
        auto instance = dynamic_cast<nota::NotaInstance*>(std::get<nota::Object*>(val));

        nota::Token prop_token{nota::TokenType::IDENTIFIER, "foo", {}, 1};
        auto prop_val = instance->get(interpreter, prop_token);
        CHECK(std::get<int>(prop_val) == 123);
    }

    SUBCASE("Methods and 'this' keyword") {
        std::string source = R"(
            class MyClass
                fn getFoo()
                    return this.foo
                end
            end

            mut instance = MyClass()
            instance.foo = 123
            let result = instance.getFoo()
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

    SUBCASE("Initializers") {
        std::string source = R"(
            class MyClass
                fn init(value)
                    this.foo = value
                end
            end

            mut instance = MyClass(456)
            let result = instance.foo
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
}
