#include "../src/parser.h"
#include <iostream>
#include <cassert>
#include <vector>

void testParseLetStatement() {
    struct TestCase {
        std::string input;
        std::string expectedIdentifier;
        std::string expectedType;
        std::string expectedValue;
    };

    std::vector<TestCase> tests = {
        {"let x = 5", "x", "", "5"},
        {"mut y = 10.5", "y", "", "10.5"},
        {"let z = \"hello\"", "z", "", "hello"},
        {"let a : int = 1", "a", "int", "1"},
        {"let b : bool = true", "b", "bool", "true"}
    };

    for (const auto& test : tests) {
        Lexer l(test.input);
        Parser p(l);
        auto program = p.parseProgram();
        assert(program != nullptr);
        assert(program->statements.size() == 1);

        auto stmt = dynamic_cast<LetStatement*>(program->statements[0].get());
        assert(stmt != nullptr);
        assert(stmt->name.literal == test.expectedIdentifier);

        if (!test.expectedType.empty()) {
            assert(stmt->type != nullptr);
            assert(stmt->type->name == test.expectedType);
        }

        assert(stmt->value->String() == test.expectedValue);
    }


    std::cout << "testParseLetStatement passed" << std::endl;
}

int main() {
    testParseLetStatement();
    return 0;
}
