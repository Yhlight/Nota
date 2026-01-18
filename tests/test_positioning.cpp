#include <gtest/gtest.h>
#include "generator.h"
#include "parser.h"
#include "lexer.h"

TEST(PositioningTest, AbsolutePositioning) {
    // Rect { x: 10; y: 20 }
    std::string source = "App { Rect { x: 10; y: 20 } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    EXPECT_NE(output.find("left: 10px;"), std::string::npos);
    EXPECT_NE(output.find("top: 20px;"), std::string::npos);
    EXPECT_NE(output.find("position: absolute;"), std::string::npos);
}

TEST(PositioningTest, ZIndex) {
    // Rect { index: 10 }
    std::string source = "App { Rect { index: 10 } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    EXPECT_NE(output.find("z-index: 10;"), std::string::npos);
}

TEST(PositioningTest, PositionProperty) {
    // Rect { position: center }
    std::string source = "App { Rect { position: center } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    EXPECT_NE(output.find("left: 50%;"), std::string::npos);
    EXPECT_NE(output.find("top: 50%;"), std::string::npos);
    EXPECT_NE(output.find("transform: translate(-50%, -50%);"), std::string::npos);
}

TEST(PositioningTest, PositionLeftTop) {
    // Rect { position: left top }
    std::string source = "App { Rect { position: left top } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    EXPECT_NE(output.find("left: 0;"), std::string::npos);
    EXPECT_NE(output.find("top: 0;"), std::string::npos);
}
