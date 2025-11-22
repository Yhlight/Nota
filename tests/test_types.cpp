#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "vendor/doctest.h"
#include <memory>
#include "test_helpers.h"

namespace doctest {
template <typename T>
struct StringMaker<std::shared_ptr<T>> {
    static String convert(const std::shared_ptr<T>& p) {
        if (p) {
            return String("shared_ptr(non-null)");
        } else {
            return String("shared_ptr(null)");
        }
    }
};
} // namespace doctest


TEST_CASE("Parser handles type annotations") {
    SUBCASE("Simple type annotation") {
        std::string source = "let x: int = 5";
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        REQUIRE(statements.size() == 1);
        auto var_stmt = std::dynamic_pointer_cast<nota::VarStmt>(statements[0]);
        REQUIRE(var_stmt);
        REQUIRE(var_stmt->type);
        CHECK(var_stmt->type->name.lexeme == "int");
        CHECK(!var_stmt->type->is_array);
    }

    SUBCASE("Dynamic array type annotation") {
        std::string source = "let x: int[]";
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        REQUIRE(statements.size() == 1);
        auto var_stmt = std::dynamic_pointer_cast<nota::VarStmt>(statements[0]);
        REQUIRE(var_stmt);
        REQUIRE(var_stmt->type);
        CHECK(var_stmt->type->name.lexeme == "int");
        CHECK(var_stmt->type->is_array);
        CHECK(var_stmt->type->size == nullptr);
    }

    SUBCASE("Static array type annotation") {
        std::string source = "let x: int[10]";
        auto lexer = std::make_shared<nota::Lexer>(source);
        auto tokens = lexer->scanTokens();
        auto parser = std::make_shared<nota::Parser>(tokens);
        auto statements = parser->parse();

        REQUIRE(statements.size() == 1);
        auto var_stmt = std::dynamic_pointer_cast<nota::VarStmt>(statements[0]);
        REQUIRE(var_stmt);
        REQUIRE(var_stmt->type);
        CHECK(var_stmt->type->name.lexeme == "int");
        CHECK(var_stmt->type->is_array);
        REQUIRE(var_stmt->type->size);
        auto size_literal = std::dynamic_pointer_cast<nota::Literal>(var_stmt->type->size);
        REQUIRE(size_literal);
        CHECK(std::get<int>(size_literal->value) == 10);
    }
}

TEST_CASE("Runtime type checking") {
    SUBCASE("Correct type assignments") {
        CHECK_NOTHROW(run("let x: int = 10"));
        CHECK_NOTHROW(run("let x: float = 10.5"));
        CHECK_NOTHROW(run("let x: bool = true"));
        CHECK_NOTHROW(run("let x: string = \"hello\""));
        CHECK_NOTHROW(run("let x: int[] = [1, 2, 3]"));
        CHECK_NOTHROW(run("let x: int = none"));
    }

    SUBCASE("Incorrect type assignments") {
        CHECK_THROWS_AS(run("let x: int = 10.5"), nota::Interpreter::RuntimeError);
        CHECK_THROWS_AS(run("let x: float = 10"), nota::Interpreter::RuntimeError);
        CHECK_THROWS_AS(run("let x: bool = 0"), nota::Interpreter::RuntimeError);
        CHECK_THROWS_AS(run("let x: string = 123"), nota::Interpreter::RuntimeError);
        CHECK_THROWS_AS(run("let x: int[] = 1"), nota::Interpreter::RuntimeError);
    }
}
