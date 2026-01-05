#include "gtest/gtest.h"
#include "Nota.h"
#include <string>

TEST(ExpressionTest, ParentReference) {
    std::string source = R"(
        App {
            id: mainApp
            width: 800

            Row {
                width: parent.width / 2
            }
        }
    )";
    Nota nota;
    std::string html_output;
    std::string css_output;
    nota.run(source, html_output, css_output);
    ASSERT_NE(css_output.find("width: 400px;"), std::string::npos);
}

TEST(ExpressionTest, ComplexExpression) {
    std::string source = R"(
        App {
            id: mainApp
            width: 100

            Row {
                id: testRow
                padding: 10
                width: mainApp.width * 2 + testRow.padding
            }
        }
    )";
    Nota nota;
    std::string html_output;
    std::string css_output;
    nota.run(source, html_output, css_output);
    ASSERT_NE(css_output.find("width: 210px;"), std::string::npos);
}
