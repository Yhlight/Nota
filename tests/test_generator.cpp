#include <gtest/gtest.h>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Generator.hpp"

TEST(GeneratorTest, GenerateSimpleAppComponent) {
    std::string source = "App { }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Program> program = parser.parse();

    ASSERT_NE(program, nullptr);

    Generator generator;
    GeneratedCode result = generator.generate(*program);

    std::string expected_html = R"(<body class="nota-app"></body>)";

    // Remove whitespace for a more robust comparison
    expected_html.erase(std::remove_if(expected_html.begin(), expected_html.end(), ::isspace), expected_html.end());
    std::string actual_html = result.html;
    actual_html.erase(std::remove_if(actual_html.begin(), actual_html.end(), ::isspace), actual_html.end());

    EXPECT_EQ(actual_html, expected_html);
}

TEST(GeneratorTest, GenerateComponentWithPropertiesAndNesting) {
    std::string source = R"(
        Row {
            spacing: 20
            color: "#333"

            Rect {
                width: 40
                height: "100%"
                color: "blue"
            }
            Text {
                text: "Hello"
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
        <div class="nota-row nota-class-0">
            <div class="nota-rect nota-class-1"></div>
            <span class="nota-text nota-class-2">Hello</span>
        </div>
    )";

    std::string expected_css = R"(
        .nota-class-0 {
            display: flex;
            flex-direction: row;
            gap: 20px;
            background-color: #333;
        }
        .nota-class-1 {
            display: block;
            width: 40px;
            height: 100%;
            background-color: blue;
        }
        .nota-class-2 {
            display: block;
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
