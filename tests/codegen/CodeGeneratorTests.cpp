#include <gtest/gtest.h>
#include "codegen/CodeGenerator.h"
#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <string>
#include <algorithm>
#include <vector>
#include <regex>

// Helper function to remove all whitespace from a string
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

// Helper function to sort CSS properties
std::string sort_css_properties(std::string css) {
    std::regex rule_regex(R"(([^\{]+)\{([^\}]+)\})");
    auto rules_begin = std::sregex_iterator(css.begin(), css.end(), rule_regex);
    auto rules_end = std::sregex_iterator();

    std::string sorted_css;
    for (std::sregex_iterator i = rules_begin; i != rules_end; ++i) {
        std::smatch match = *i;
        std::string selector = match[1].str();
        std::string properties_str = match[2].str();

        std::regex prop_regex(R"(([^:]+:[^;]+;))");
        auto props_begin = std::sregex_iterator(properties_str.begin(), properties_str.end(), prop_regex);
        auto props_end = std::sregex_iterator();

        std::vector<std::string> properties;
        for (std::sregex_iterator j = props_begin; j != props_end; ++j) {
            properties.push_back((*j).str(0));
        }

        std::sort(properties.begin(), properties.end());

        sorted_css += selector + "{";
        for (const auto& prop : properties) {
            sorted_css += prop;
        }
        sorted_css += "}";
    }
    return sorted_css;
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

    std::regex style_regex(R"(<style>.*</style>)");
    std::smatch actual_match, expected_match;
    std::regex_search(html_output, actual_match, style_regex);
    std::regex_search(expected_html, expected_match, style_regex);

    std::string actual_style = actual_match.str(0);
    std::string expected_style = expected_match.str(0);

    EXPECT_EQ(sort_css_properties(remove_whitespace(actual_style)), sort_css_properties(remove_whitespace(expected_style)));
}

TEST(CodeGeneratorTest, GeneratePositioningProperties) {
    std::string source = R"(
        App {
            width: 800
            height: 600

            Rect {
                x: 100
                y: 50
                index: 2
                width: 50
                height: 50
                color: "red"
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    // Use string::find for more robust checking, ignoring exact whitespace.
    EXPECT_TRUE(html_output.find("position: relative;") != std::string::npos);
    EXPECT_TRUE(html_output.find("position: absolute;") != std::string::npos);
    EXPECT_TRUE(html_output.find("left: 100px;") != std::string::npos);
    EXPECT_TRUE(html_output.find("top: 50px;") != std::string::npos);
    EXPECT_TRUE(html_output.find("z-index: 2;") != std::string::npos);
}

TEST(CodeGeneratorTest, MergeDuplicateCssRules) {
    std::string source = R"(
        App {
            width: 800
            height: 600

            Rect {
                width: 50
                height: 50
                color: "blue"
            }

            Rect {
                width: 50
                height: 50
                color: "blue"
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();

    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string html_output = generator.generate(ast);

    // 1. Check that only one CSS rule for a Rect component is generated.
    std::string rect_class_prefix = ".Rect-";
    size_t first_pos = html_output.find(rect_class_prefix);
    ASSERT_NE(first_pos, std::string::npos);
    size_t second_pos = html_output.find(rect_class_prefix, first_pos + 1);
    EXPECT_EQ(second_pos, std::string::npos);

    // 2. Check that the generated rule contains the correct properties.
    std::string style_content;
    size_t style_start = html_output.find("<style>");
    size_t style_end = html_output.find("</style>");
    if (style_start != std::string::npos && style_end != std::string::npos) {
        style_content = html_output.substr(style_start, style_end - style_start);
    }
    EXPECT_TRUE(style_content.find("width: 50px;") != std::string::npos);
    EXPECT_TRUE(style_content.find("height: 50px;") != std::string::npos);
    EXPECT_TRUE(style_content.find("background-color: blue;") != std::string::npos);

    // 3. Extract class name and check that both divs use it.
    std::regex class_regex(R"(\.(Rect-\d+)\s*\{)");
    std::smatch match;
    ASSERT_TRUE(std::regex_search(html_output, match, class_regex));
    std::string class_name = match[1].str();

    std::string expected_divs = "<divclass=\"" + class_name + "\"></div><divclass=\"" + class_name + "\"></div>";

    // Remove whitespace to make the comparison robust
    std::string body_content = html_output.substr(html_output.find("<body"));
    body_content.erase(std::remove_if(body_content.begin(), body_content.end(), ::isspace), body_content.end());

    EXPECT_TRUE(body_content.find(expected_divs) != std::string::npos);
}

TEST(CodeGeneratorTest, PositionProperty) {
    std::string source = R"(
        App {
            Rect {
                position: center;
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string output = generator.generate(ast);

    EXPECT_NE(output.find("left: 50%;"), std::string::npos);
    EXPECT_NE(output.find("top: 50%;"), std::string::npos);
    EXPECT_NE(output.find("transform: translate(-50%, -50%);"), std::string::npos);
    EXPECT_NE(output.find("position: absolute;"), std::string::npos);
    EXPECT_NE(output.find("position: relative;"), std::string::npos);
}

TEST(CodeGeneratorTest, CustomClassProperty) {
    std::string source = R"(
        App {
            Rect {
                class: "my-custom-class";
            }
        }
    )";

    Lexer lexer(source);
    Parser parser(lexer);
    RootNode ast = parser.parse();
    ASSERT_TRUE(parser.errors().empty());

    CodeGenerator generator;
    std::string output = generator.generate(ast);

    std::regex class_regex(R"(class="Rect-\d+ my-custom-class")");
    EXPECT_TRUE(std::regex_search(output, class_regex));
}
