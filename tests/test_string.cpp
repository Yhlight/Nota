#include "doctest.h"
#include "test_helpers.h"

TEST_CASE("String Operations") {
    SUBCASE("String concatenation") {
        std::string code = R"(
            let a = "hello"
            let b = " world"
            return a + b
        )";
        auto result = run(code);
        REQUIRE(std::holds_alternative<std::string>(result));
        CHECK(std::get<std::string>(result) == "hello world");
    }

    SUBCASE("String subscript read access") {
        std::string code = R"(
            let a = "hello"
            return a[1]
        )";
        auto result = run(code);
        REQUIRE(std::holds_alternative<std::string>(result));
        CHECK(std::get<std::string>(result) == "e");
    }

    SUBCASE("String subscript write access is an error") {
        std::string code = R"(
            let a = "hello"
            a[1] = "a"
        )";
        CHECK_THROWS_WITH_MESSAGE(run(code), "Strings are immutable.", "Should throw for string immutability");
    }

    SUBCASE("String subscript out of bounds") {
        std::string code = R"(
            let a = "hello"
            return a[10]
        )";
        CHECK_THROWS_WITH_MESSAGE(run(code), "String index out of bounds.", "Should throw for out of bounds access");
    }
}
