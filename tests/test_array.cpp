#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>
#include "test_helpers.h"

TEST_CASE("Array literals") {
    std::string source = "let a = [1, \"two\", true]";
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
    REQUIRE(std::holds_alternative<nota::Object*>(a_val));
    auto obj = std::get<nota::Object*>(a_val);
    REQUIRE(obj->type == nota::ObjectType::ARRAY);
    auto array = static_cast<nota::NotaArray*>(obj);
    REQUIRE(array->elements.size() == 3);
    CHECK(std::get<int>(array->elements[0]) == 1);
    CHECK(std::get<nota::Object*>(array->elements[1])->type == nota::ObjectType::STRING);
    auto str_obj = static_cast<nota::NotaString*>(std::get<nota::Object*>(array->elements[1]));
    CHECK(str_obj->value == "two");
    CHECK(std::get<bool>(array->elements[2]) == true);
}

TEST_CASE("Array subscript access") {
    std::string source = R"(
        let a = [1, 2, 3]
        let b = a[1]
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
    CHECK(std::get<int>(b_val) == 2);
}

TEST_CASE("Array subscript assignment") {
    std::string source = R"(
        let a = [1, 2, 3]
        a[1] = 99
    )";
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
    auto obj = std::get<nota::Object*>(a_val);
    auto array = static_cast<nota::NotaArray*>(obj);
    CHECK(std::get<int>(array->elements[1]) == 99);
}

TEST_CASE("Array index out of bounds") {
    std::string source = "let a = [1, 2, 3]; a[3]";
    nota::VM vm;
    auto interpreter = std::make_shared<nota::Interpreter>(vm);
    auto lexer = std::make_shared<nota::Lexer>(source);
    auto tokens = lexer->scanTokens();
    auto parser = std::make_shared<nota::Parser>(tokens);
    auto statements = parser->parse();
    CHECK_THROWS_AS(interpreter->interpret(statements), nota::Interpreter::RuntimeError);
}

TEST_CASE("Static arrays") {
    SUBCASE("Initialization within capacity") {
        CHECK_NOTHROW(run("let a: int[3] = [1, 2, 3]"));
    }

    SUBCASE("Initialization exceeding capacity") {
        CHECK_THROWS_AS(run("let a: int[2] = [1, 2, 3]"), nota::Interpreter::RuntimeError);
    }

    SUBCASE("Subscript access within capacity") {
        CHECK(std::get<int>(run("let a: int[3] = [10, 20]; let result = a[1]")) == 20);
    }

    SUBCASE("Subscript access out of capacity") {
        CHECK_THROWS_AS(run("let a: int[2] = [1]; let result = a[2]"), nota::Interpreter::RuntimeError);
    }

    SUBCASE("Subscript assignment within capacity") {
        CHECK(std::get<int>(run("let a: int[3] = [1]; a[2] = 99; let result = a[2]")) == 99);
    }

    SUBCASE("Subscript assignment out of capacity") {
        CHECK_THROWS_AS(run("let a: int[2] = [1]; a[2] = 99"), nota::Interpreter::RuntimeError);
    }
}

TEST_CASE("Array element type checking") {
    SUBCASE("Correct element types") {
        CHECK_NOTHROW(run("let a: int[] = [1, 2, 3]"));
        CHECK_NOTHROW(run("let a: float[] = [1.0, 2, 3.5]")); // Implicit conversion
    }

    SUBCASE("Incorrect element types") {
        CHECK_THROWS_AS(run("let a: int[] = [1, \"two\", 3]"), nota::Interpreter::RuntimeError);
    }

    SUBCASE("Assignment with correct element type") {
        CHECK_NOTHROW(run("let a: int[] = [1]; a[0] = 2"));
    }

    SUBCASE("Assignment with incorrect element type") {
        CHECK_THROWS_AS(run("let a: int[] = [1]; a[0] = \"hello\""), nota::Interpreter::RuntimeError);
    }
}

TEST_CASE("Empty array") {
    std::string source = "let a = []";
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
    REQUIRE(std::holds_alternative<nota::Object*>(a_val));
    auto obj = std::get<nota::Object*>(a_val);
    REQUIRE(obj->type == nota::ObjectType::ARRAY);
    auto array = static_cast<nota::NotaArray*>(obj);
    REQUIRE(array->elements.size() == 0);
}

TEST_CASE("Nested arrays") {
    std::string source = "let a = [1, [2, 3]]";
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
    auto obj = std::get<nota::Object*>(a_val);
    auto array = static_cast<nota::NotaArray*>(obj);
    REQUIRE(array->elements.size() == 2);
    auto nested_obj = std::get<nota::Object*>(array->elements[1]);
    REQUIRE(nested_obj->type == nota::ObjectType::ARRAY);
    auto nested_array = static_cast<nota::NotaArray*>(nested_obj);
    REQUIRE(nested_array->elements.size() == 2);
    CHECK(std::get<int>(nested_array->elements[0]) == 2);
    CHECK(std::get<int>(nested_array->elements[1]) == 3);
}

TEST_CASE("Array index with negative number") {
    std::string source = "let a = [1, 2, 3]; a[-1]";
    nota::VM vm;
    auto interpreter = std::make_shared<nota::Interpreter>(vm);
    auto lexer = std::make_shared<nota::Lexer>(source);
    auto tokens = lexer->scanTokens();
    auto parser = std::make_shared<nota::Parser>(tokens);
    auto statements = parser->parse();
    CHECK_THROWS_AS(interpreter->interpret(statements), nota::Interpreter::RuntimeError);
}
