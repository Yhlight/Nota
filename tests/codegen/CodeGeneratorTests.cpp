#include <gtest/gtest.h>
#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"

// Helper function to remove all whitespace from a string
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

TEST(CodeGeneratorTest, GenerateSimpleComponent) {
    std::string source = R"(
        App {
            width: 800
            height: 600
            color: "#eee"

            Row {
                spacing: 10

                Rect {
                    width: 50
                    height: 50
                    color: "blue"
                }

                Text {
                    text: "Welcome"
                    color: "black"
                }
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    std::string expected_html = R"(
        <!DOCTYPE html>
        <html>
        <head>
        <style>
        /* Basic Reset */
        body, div, span { margin: 0; padding: 0; box-sizing: border-box; overflow: hidden; }
        .App-0 {
            width: 800px;
            height: 600px;
            background-color: #eee;
        }
        .Row-1 {
            display: flex;
            flex-direction: row;
            gap: 10px;
        }
        .Rect-2 {
            width: 50px;
            height: 50px;
            background-color: blue;
        }
        .Text-3 {
            color: black;
        }
        </style>
        </head>
        <body class="App-0">
        <div class="Row-1"><div class="Rect-2"></div><span class="Text-3">Welcome</span></div></body>
        </html>
    )";

    // Add a newline to the expected output to match the generator's new behavior
    expected_html += "\n";

    EXPECT_EQ(remove_whitespace(html_output), remove_whitespace(expected_html));
}
