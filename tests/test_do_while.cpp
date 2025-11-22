#include "doctest.h"
#include "test_helpers.h"

TEST_CASE("Do-While Loop") {
    SUBCASE("Scope persists across iterations") {
        std::string code = R"(
            mut a = 0
            mut total = 0
            do
                if a == 0
                    mut b = 1
                end
                total = total + b
                b = b + 1
                a = a + 1
            while a < 2
            return total
        )";
        auto result = run(code);
        REQUIRE(std::holds_alternative<int>(result));
        CHECK(std::get<int>(result) == 3);
    }
}
