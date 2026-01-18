#include <gtest/gtest.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Generator.hpp"
#include "ComponentRegistry.hpp"

class ComponentTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear the registry before each test to ensure a clean state
        ComponentRegistry::getInstance().reset();
    }
};

TEST_F(ComponentTest, DefineAndInstantiateCustomComponent) {
    std::string source = R"(
        Item Box {
            Rect {
                width: 100
                height: 100
                color: "red"
            }
        }

        App {
            Box { }
            Box {
                color: "blue"
            }
        }
    )";

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Program> program = parser.parse();

    ASSERT_NE(program, nullptr);

    Generator generator;
    GeneratedCode result = generator.generate(*program);

    std::string expected_html = R"(
        <body class="nota-app">
            <div class="nota-rect nota-class-0"></div>
            <div class="nota-rect nota-class-1"></div>
        </body>
    )";

    std::string expected_css = R"(
        .nota-class-0 {
            display: block;
            width: 100px;
            height: 100px;
            background-color: red;
        }
        .nota-class-1 {
            display: block;
            width: 100px;
            height: 100px;
            background-color: blue;
        }
    )";

    // Normalize whitespace for a more robust comparison
    auto normalize = [](std::string& s) {
        s.erase(std::remove(s.begin(), s.end(), '\n'), s.end());
        s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
        s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
    };

    std::string actual_html = result.html;
    std::string actual_css = result.css;

    normalize(expected_html);
    normalize(expected_css);
    normalize(actual_html);
    normalize(actual_css);

    EXPECT_EQ(actual_html, expected_html);
    EXPECT_EQ(actual_css, expected_css);
}
