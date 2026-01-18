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
    EXPECT_NE(output.find("<body class=\"nota-app\">"), std::string::npos);
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

    // Check tag is span
    EXPECT_NE(output.find("<span class=\"nota-text-"), std::string::npos);
    // Check content
    EXPECT_NE(output.find(">Hello World</span>"), std::string::npos);
    // Check style (background-color, not text color? Nota.md says color: white background of box?)
    // Nota.md: Text { text: "Nota Dashboard"; color: white; }
    // If it's the text color, it should be "color: white;". If it's background, "background-color".
    // Nota.md "基本组件" section for Rect says "color => background color".
    // For Text? "Text... 表示一个可控的文本组件".
    // Usually color in Text implies font color.
    // But currently my generator maps "color" to "background-color".
    // Let's check Nota.md details on properties.
    // "color => background color"
    // "text => 容器的文本"
    // It doesn't mention font color.
    // Assuming for now "color" is background color as per general property list.
    // Maybe I should add "fontColor" or check if "color" behaves differently for Text?
    // In many UI frameworks, color on Text is font color.
    // But Nota.md explicitly says "color => 背景颜色".
    // So I will stick to background-color for now.

    EXPECT_NE(output.find("background-color: white;"), std::string::npos);
}

TEST(GeneratorTest, GenerateProperties) {
    auto rect = std::make_shared<nota::Component>("Rect");
    rect->addProperty(std::make_shared<nota::Property>("padding", std::make_shared<nota::Literal>(20)));
    rect->addProperty(std::make_shared<nota::Property>("radius", std::make_shared<nota::Literal>(10)));

    nota::Generator generator;
    std::string output = generator.generate(rect);

    EXPECT_NE(output.find("padding: 20px;"), std::string::npos);
    EXPECT_NE(output.find("border-radius: 10px;"), std::string::npos);
    EXPECT_NE(output.find("display: block;"), std::string::npos);
}
