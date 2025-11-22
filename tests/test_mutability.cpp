#include "doctest.h"
#include "test_helpers.h"

TEST_CASE("Mutability") {
    SUBCASE("let variables are immutable") {
        std::string code = R"(
            let a = 10
            a = 20
        )";
        CHECK_THROWS_WITH_MESSAGE(run(code), "Cannot assign to immutable variable 'a'.", "Should throw for immutable assignment");
    }

    SUBCASE("mut variables are mutable") {
        std::string code = R"(
            mut a = 10
            a = 20
            return a
        )";
        auto result = run(code);
        CHECK(std::get<int>(result) == 20);
    }

    SUBCASE("Reassigning a function is an error") {
        std::string code = R"(
            fn my_func()
            end
            my_func = 10
        )";
        CHECK_THROWS_WITH_MESSAGE(run(code), "Cannot assign to immutable variable 'my_func'.", "Should throw for function reassignment");
    }

    SUBCASE("Reassigning a class is an error") {
        std::string code = R"(
            class MyClass
            end
            MyClass = 10
        )";
        CHECK_THROWS_WITH_MESSAGE(run(code), "Cannot assign to immutable variable 'MyClass'.", "Should throw for class reassignment");
    }

    SUBCASE("For-each loop variable is immutable") {
        std::string code = R"(
            let a = [1, 2, 3]
            for let i : a
                i = 10
            end
        )";
        CHECK_THROWS_WITH_MESSAGE(run(code), "Cannot assign to immutable variable 'i'.", "Should throw for immutable for-each variable");
    }
}
