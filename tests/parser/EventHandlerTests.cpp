#include "parser/Parser.h"
#include "lexer/Lexer.h"
#include <gtest/gtest.h>
#include <string>
#include <algorithm>
#include <cctype>

// Helper function to trim whitespace from both ends of a string
static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

TEST(ParserTest, ParseComponentWithEventHandler) {
    std::string source = R"(
        App {
            onClick: {
                console.log("Clicked!");
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    RootNode root = parser.parse();

    ASSERT_TRUE(root.root_component);
    ASSERT_EQ(root.root_component->event_handlers.size(), 1);
    EXPECT_EQ(root.root_component->event_handlers[0].name.text, "onClick");
    EXPECT_EQ(trim(root.root_component->event_handlers[0].body), "console.log(\"Clicked!\");");
}

TEST(ParserTest, EventHandlersNotAllowedInItems) {
    std::string source = R"(
        Item MyItem {
            onClick: {
                // some code
            }
        }
    )";
    Lexer lexer(source);
    Parser parser(lexer);
    parser.parse();

    ASSERT_FALSE(parser.errors().empty());
    EXPECT_EQ(parser.errors()[0].message, "Event handlers are not allowed in Item definitions.");
}
