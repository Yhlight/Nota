#include <gtest/gtest.h>
#include "parser/Parser.hpp"
#include "core/AST.hpp"
#include <vector>
#include <variant>

using namespace nota::core;
using namespace nota::parser;
using namespace nota::lexer;

TEST(ParserTest, LetStatements) {
    std::string source = R"(
        let x = 5
        let y = 10
        let foobar = 838383
    )";

    Lexer l(source);
    Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty());
    ASSERT_EQ(program->statements.size(), 3);

    std::vector<std::string> expected_identifiers = {"x", "y", "foobar"};

    for (size_t i = 0; i < program->statements.size(); ++i) {
        auto stmt = program->statements[i].get();
        auto let_stmt = static_cast<LetStatement*>(stmt);
        EXPECT_EQ(let_stmt->name->value, expected_identifiers[i]);
    }
}

TEST(ParserTest, MutStatements) {
    std::string source = R"(
        mut x = 5
        mut y = 10
        mut foobar = 838383
    )";

    Lexer l(source);
    Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty());
    ASSERT_EQ(program->statements.size(), 3);

    std::vector<std::string> expected_identifiers = {"x", "y", "foobar"};

    for (size_t i = 0; i < program->statements.size(); ++i) {
        auto stmt = program->statements[i].get();
        auto mut_stmt = static_cast<MutStatement*>(stmt);
        EXPECT_EQ(mut_stmt->name->value, expected_identifiers[i]);
    }
}

TEST(ParserTest, IdentifierExpression) {
    std::string source = "foobar";
    Lexer l(source);
    Parser p(l);
    auto program = p.ParseProgram();
    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty());
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = static_cast<ExpressionStatement*>(program->statements[0].get());
    auto ident = static_cast<Identifier*>(stmt->expression.get());
    ASSERT_NE(ident, nullptr);
    EXPECT_EQ(ident->value, "foobar");
}

TEST(ParserTest, IntegerLiteralExpression) {
    std::string source = "5";
    Lexer l(source);
    Parser p(l);
    auto program = p.ParseProgram();
    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty());
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = static_cast<ExpressionStatement*>(program->statements[0].get());
    auto literal = static_cast<IntegerLiteral*>(stmt->expression.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 5);
}

TEST(ParserTest, BooleanExpression) {
    std::vector<std::pair<std::string, bool>> tests = {
        {"true", true},
        {"false", false},
    };

    for (const auto& test : tests) {
        Lexer l(test.first);
        Parser p(l);
        auto program = p.ParseProgram();
        ASSERT_NE(program, nullptr);
        ASSERT_TRUE(p.Errors().empty());
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = static_cast<ExpressionStatement*>(program->statements[0].get());
        auto boolean = static_cast<Boolean*>(stmt->expression.get());
        ASSERT_NE(boolean, nullptr);
        EXPECT_EQ(boolean->value, test.second);
    }
}

TEST(ParserTest, PrefixExpressions) {
    std::vector<std::pair<std::string, std::string>> prefix_tests = {
        {"!5", "!"},
        {"-15", "-"},
    };

    for (const auto& test : prefix_tests) {
        Lexer l(test.first);
        Parser p(l);
        auto program = p.ParseProgram();
        ASSERT_NE(program, nullptr);
        ASSERT_TRUE(p.Errors().empty());
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = static_cast<ExpressionStatement*>(program->statements[0].get());
        auto prefix_expr = static_cast<PrefixExpression*>(stmt->expression.get());
        ASSERT_NE(prefix_expr, nullptr);
        EXPECT_EQ(prefix_expr->op, test.second);

        auto literal = static_cast<IntegerLiteral*>(prefix_expr->right.get());
        ASSERT_NE(literal, nullptr);
    }
}

TEST(ParserTest, InfixExpressions) {
    std::vector<std::tuple<std::string, int64_t, std::string, int64_t>> infix_tests = {
        {"5 + 5", 5, "+", 5},
        {"5 - 5", 5, "-", 5},
        {"5 * 5", 5, "*", 5},
        {"5 / 5", 5, "/", 5},
        {"5 > 5", 5, ">", 5},
        {"5 < 5", 5, "<", 5},
        {"5 == 5", 5, "==", 5},
        {"5 != 5", 5, "!=", 5},
    };

    for (const auto& test : infix_tests) {
        Lexer l(std::get<0>(test));
        Parser p(l);
        auto program = p.ParseProgram();
        ASSERT_NE(program, nullptr);
        ASSERT_TRUE(p.Errors().empty());
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = static_cast<ExpressionStatement*>(program->statements[0].get());
        auto infix_expr = static_cast<InfixExpression*>(stmt->expression.get());
        ASSERT_NE(infix_expr, nullptr);

        auto left_literal = static_cast<IntegerLiteral*>(infix_expr->left.get());
        ASSERT_NE(left_literal, nullptr);
        EXPECT_EQ(left_literal->value, std::get<1>(test));

        EXPECT_EQ(infix_expr->op, std::get<2>(test));

        auto right_literal = static_cast<IntegerLiteral*>(infix_expr->right.get());
        ASSERT_NE(right_literal, nullptr);
        EXPECT_EQ(right_literal->value, std::get<3>(test));
    }
}

TEST(ParserTest, OperatorPrecedence) {
    std::vector<std::pair<std::string, std::string>> tests = {
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"},
        {"(5 + 5) * 2", "((5 + 5) * 2)"},
        {"2 / (5 + 5)", "(2 / (5 + 5))"},
        {"-(5 + 5)", "(-(5 + 5))"},
        {"!(true == true)", "(!(true == true))"},
    };

    for (const auto& test : tests) {
        Lexer l(test.first);
        Parser p(l);
        auto program = p.ParseProgram();
        ASSERT_NE(program, nullptr);
        ASSERT_TRUE(p.Errors().empty());

        EXPECT_EQ(program->ToString(), test.second);
    }
}

TEST(ParserTest, MultiLineExpressions) {
    std::string source = R"(
        let a = 10 +
                20 +
                30
        let b = (
            10 +
            20 +
            30
        )
    )";

    Lexer l(source);
    Parser p(l);
    auto program = p.ParseProgram();
    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty());
    ASSERT_EQ(program->statements.size(), 2);

    auto stmt1 = static_cast<LetStatement*>(program->statements[0].get());
    EXPECT_EQ(stmt1->name->value, "a");
    auto infix_expr1 = static_cast<InfixExpression*>(stmt1->value.get());
    EXPECT_EQ(infix_expr1->op, "+");
    auto right1 = static_cast<IntegerLiteral*>(infix_expr1->right.get());
    EXPECT_EQ(right1->value, 30);
    auto left1 = static_cast<InfixExpression*>(infix_expr1->left.get());
    EXPECT_EQ(left1->op, "+");
    auto left1_left = static_cast<IntegerLiteral*>(left1->left.get());
    EXPECT_EQ(left1_left->value, 10);
    auto left1_right = static_cast<IntegerLiteral*>(left1->right.get());
    EXPECT_EQ(left1_right->value, 20);

    auto stmt2 = static_cast<LetStatement*>(program->statements[1].get());
    EXPECT_EQ(stmt2->name->value, "b");
    auto infix_expr2 = static_cast<InfixExpression*>(stmt2->value.get());
    EXPECT_EQ(infix_expr2->op, "+");
    auto right2 = static_cast<IntegerLiteral*>(infix_expr2->right.get());
    EXPECT_EQ(right2->value, 30);
    auto left2 = static_cast<InfixExpression*>(infix_expr2->left.get());
    EXPECT_EQ(left2->op, "+");
    auto left2_left = static_cast<IntegerLiteral*>(left2->left.get());
    EXPECT_EQ(left2_left->value, 10);
    auto left2_right = static_cast<IntegerLiteral*>(left2->right.get());
    EXPECT_EQ(left2_right->value, 20);
}
