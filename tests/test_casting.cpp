#include "vendor/doctest.h"
#include "test_helpers.h"

TEST_CASE("Casting") {
    SUBCASE("to int") {
        CHECK(std::get<int>(run("let result = (int)10.5")) == 10);
        CHECK(std::get<int>(run("let result = (int)true")) == 1);
        CHECK(std::get<int>(run("let result = (int)\"123\"")) == 123);
        CHECK_THROWS_AS(run("let result = (int)\"abc\""), nota::Interpreter::RuntimeError);
    }

    SUBCASE("to float") {
        CHECK(std::get<double>(run("let result = (float)10")) == 10.0);
        CHECK(std::get<double>(run("let result = (float)true")) == 1.0);
        CHECK(std::get<double>(run("let result = (float)\"123.4\"")) == 123.4);
        CHECK_THROWS_AS(run("let result = (float)\"abc\""), nota::Interpreter::RuntimeError);
    }

    SUBCASE("to string") {
        CHECK(std::get<std::string>(run("let result = (string)10")) == "10");
        CHECK(std::get<std::string>(run("let result = (string)123.4")) == "123.400000");
        CHECK(std::get<std::string>(run("let result = (string)true")) == "true");
    }

    SUBCASE("to bool") {
        CHECK(std::get<bool>(run("let result = (bool)1")) == true);
        CHECK(std::get<bool>(run("let result = (bool)0")) == false);
        CHECK(std::get<bool>(run("let result = (bool)12.3")) == true);
        CHECK(std::get<bool>(run("let result = (bool)0.0")) == false);
        CHECK(std::get<bool>(run("let result = (bool)\"abc\"")) == true);
        CHECK(std::get<bool>(run("let result = (bool)\"\"")) == true); // Non-empty strings are truthy
        CHECK(std::get<bool>(run("let result = (bool)none")) == false);
    }

    SUBCASE("Invalid cast") {
        CHECK_THROWS_AS(run("let result = (invalid)10"), nota::Interpreter::RuntimeError);
        CHECK_THROWS_AS(run("let result = (int)[1, 2]"), nota::Interpreter::RuntimeError);
    }

    SUBCASE("Array cast") {
        CHECK_NOTHROW(run("let result = (int[])[1, 2]"));
        CHECK_THROWS_AS(run("let result = (int[])1"), nota::Interpreter::RuntimeError);
    }
}
