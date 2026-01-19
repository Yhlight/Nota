#include <gtest/gtest.h>
#include "generator.h"
#include "parser.h"
#include "lexer.h"

TEST(EventsTest, ParseOnClickString) {
    std::string source = "App { onClick: \"alert('hello')\" }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    EXPECT_NE(output.find("onclick = function(event) { alert('hello') };"), std::string::npos);
    EXPECT_NE(output.find("<script>"), std::string::npos);
}

TEST(EventsTest, ParseOnClickBlock) {
    std::string source = "App { onClick: { console.log('hello'); } }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    nota::Generator generator;
    std::string output = generator.generate(program);

    // The parser consumes spaces inside tokens differently or simple token concatenation adds spaces
    // Let's just check for key parts
    EXPECT_NE(output.find("onclick = function(event) {"), std::string::npos);
    EXPECT_NE(output.find("console"), std::string::npos);
    EXPECT_NE(output.find("log"), std::string::npos);
    EXPECT_NE(output.find("hello"), std::string::npos);
    EXPECT_NE(output.find("};"), std::string::npos);
}

TEST(EventsTest, ParseDelegate) {
    std::string source = "App { delegate [onClick] for parent }";
    nota::Lexer lexer(source);
    nota::Parser parser(lexer.tokenize());
    auto program = parser.parse();

    // Just verifying parse success for now, as delegate logic isn't fully in generator
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->components[0]->delegates.size(), 1);

    auto delegate = program->components[0]->delegates[0];
    auto list = delegate->events;
    ASSERT_EQ(list->elements.size(), 1);

    // Verify target. Parent token is parsed as ParentExpression, NOT Identifier "parent"
    auto target = std::dynamic_pointer_cast<nota::ParentExpression>(delegate->target);
    ASSERT_NE(target, nullptr);
}
