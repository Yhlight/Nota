#include "vendor/doctest.h"
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"
#include "NotaObjects.h"
#include "test_helpers.h"
#include "VM.h"
#include <memory>
#include "test_helpers.h"

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

    SUBCASE("Parse typed properties") {
        std::string source = R"(
            class MyClass
                foo: int
                bar: string[]
            end
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();

        REQUIRE(statements.size() == 1);
        auto class_stmt = std::dynamic_pointer_cast<nota::ClassStmt>(statements[0]);
        REQUIRE(class_stmt);
        REQUIRE(class_stmt->properties.size() == 2);

        auto prop1 = class_stmt->properties[0];
        CHECK(prop1->name.lexeme == "foo");
        REQUIRE(prop1->type);
        CHECK(prop1->type->name.lexeme == "int");
        CHECK(!prop1->type->is_array);

        auto prop2 = class_stmt->properties[1];
        CHECK(prop2->name.lexeme == "bar");
        REQUIRE(prop2->type);
        CHECK(prop2->type->name.lexeme == "string");
        CHECK(prop2->type->is_array);
        CHECK(prop2->type->size == nullptr);
    }

    SUBCASE("Static methods") {
        std::string source = R"(
            class MyClass
                static fn sayHello()
                    return "Hello"
                end
            end

            let result = MyClass::sayHello()
        )";

        auto result = run(source);
        REQUIRE(std::holds_alternative<std::string>(result));
        CHECK(std::get<std::string>(result) == "Hello");
    }

    SUBCASE("Property type checking") {
        std::string source = R"(
            class MyClass
                foo: int
                bar: string
            end

            let instance = MyClass()
        )";

        SUBCASE("Correct assignments") {
            CHECK_NOTHROW(run(source + "instance.foo = 123"));
            CHECK_NOTHROW(run(source + "instance.bar = \"hello\""));
            CHECK_NOTHROW(run(source + "instance.foo = 12.3")); // implicit conversion
        }

        SUBCASE("Incorrect assignments") {
            CHECK_THROWS_AS(run(source + "instance.foo = \"hello\""), nota::Interpreter::RuntimeError);
            CHECK_THROWS_AS(run(source + "instance.bar = 123"), nota::Interpreter::RuntimeError);
        }
    }
}
