#include <gtest/gtest.h>
#include "generator.h"
#include "ast.h"

TEST(GeneratorTest, GenerateApp) {
    auto app = std::make_shared<nota::Component>("App");
    app->addProperty(std::make_shared<nota::Property>("width", std::make_shared<nota::Literal>(100)));

    nota::Generator generator;
    std::string output = generator.generate(app);

    EXPECT_NE(output.find(".nota-app {"), std::string::npos);
    EXPECT_NE(output.find("width: 100px;"), std::string::npos);
    EXPECT_NE(output.find("<body class=\"nota-app\""), std::string::npos);
}

TEST(GeneratorTest, GenerateRowCol) {
    auto row = std::make_shared<nota::Component>("Row");
    row->addProperty(std::make_shared<nota::Property>("spacing", std::make_shared<nota::Literal>(10)));

    nota::Generator generator;
    std::string output = generator.generate(row);

    EXPECT_NE(output.find("display: flex;"), std::string::npos);
    EXPECT_NE(output.find("flex-direction: row;"), std::string::npos);
    EXPECT_NE(output.find("gap: 10px;"), std::string::npos);

    auto col = std::make_shared<nota::Component>("Col");
    output = generator.generate(col);
    EXPECT_NE(output.find("flex-direction: column;"), std::string::npos);
}

TEST(GeneratorTest, GenerateText) {
    auto text = std::make_shared<nota::Component>("Text");
    text->addProperty(std::make_shared<nota::Property>("text", std::make_shared<nota::Literal>("Hello World")));
    text->addProperty(std::make_shared<nota::Property>("color", std::make_shared<nota::Literal>("white")));

    nota::Generator generator;
    std::string output = generator.generate(text);

    EXPECT_NE(output.find("<span class=\"nota-text-"), std::string::npos);
    EXPECT_NE(output.find(">Hello World</span>"), std::string::npos);
    EXPECT_NE(output.find("color: white;"), std::string::npos);
}

TEST(GeneratorTest, GenerateProperties) {
    auto rect = std::make_shared<nota::Component>("Rect");
    rect->addProperty(std::make_shared<nota::Property>("padding", std::make_shared<nota::Literal>(20)));
    rect->addProperty(std::make_shared<nota::Property>("radius", std::make_shared<nota::Literal>(10)));

    nota::Generator generator;
    std::string output = generator.generate(rect);

    EXPECT_NE(output.find("padding: 20px;"), std::string::npos);
    EXPECT_NE(output.find("border-radius: 10px;"), std::string::npos);
}

TEST(GeneratorTest, GenerateIdAndCalc) {
    auto rect = std::make_shared<nota::Component>("Rect");
    rect->addProperty(std::make_shared<nota::Property>("id", std::make_shared<nota::Literal>("myRect")));

    // width: 100 + 50
    auto bin = std::make_shared<nota::BinaryExpression>(
        std::make_shared<nota::Literal>(100),
        "+",
        std::make_shared<nota::Literal>(50)
    );
    rect->addProperty(std::make_shared<nota::Property>("width", bin));

    nota::Generator generator;
    std::string output = generator.generate(rect);

    EXPECT_NE(output.find("id=\"myRect\""), std::string::npos);
    EXPECT_NE(output.find("width: calc(100px + 50px);"), std::string::npos);
}

TEST(GeneratorTest, GenerateMemberAccess) {
    auto rect = std::make_shared<nota::Component>("Rect");

    // width: box.width
    auto mem = std::make_shared<nota::MemberExpression>(
        std::make_shared<nota::Identifier>("box"),
        "width"
    );
    rect->addProperty(std::make_shared<nota::Property>("width", mem));

    nota::Generator generator;
    std::string output = generator.generate(rect);

    // For now we just check simple stringification
    EXPECT_NE(output.find("width: box.width;"), std::string::npos);
}
