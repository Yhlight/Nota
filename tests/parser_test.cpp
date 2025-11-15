#include "gtest/gtest.h"
#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <string>
#include <iostream>

// ASTPrinter implementation for testing
class ASTPrinter : public ExprVisitor {
public:
    std::string print(std::shared_ptr<Expr> expr) {
        expr->accept(*this);
        return result;
    }

    void visit(const std::shared_ptr<BinaryExpr>& expr) override {
        result += "(";
        result += expr->op.lexeme;
        result += " ";
        expr->left->accept(*this);
        result += " ";
        expr->right->accept(*this);
        result += ")";
    }

    void visit(const std::shared_ptr<GroupingExpr>& expr) override {
        result += "(group ";
        expr->expression->accept(*this);
        result += ")";
    }

    void visit(const std::shared_ptr<LiteralExpr>& expr) override {
        if (std::holds_alternative<std::string>(expr->value)) {
            result += std::get<std::string>(expr->value);
        } else if (std::holds_alternative<long long>(expr->value)) {
            result += std::to_string(std::get<long long>(expr->value));
        } else if (std::holds_alternative<double>(expr->value)) {
            result += std::to_string(std::get<double>(expr->value));
        } else if (std::holds_alternative<bool>(expr->value)) {
            result += std::get<bool>(expr->value) ? "true" : "false";
        } else {
            result += "nil";
        }
    }

    void visit(const std::shared_ptr<UnaryExpr>& expr) override {
        result += "(";
        result += expr->op.lexeme;
        result += " ";
        expr->right->accept(*this);
        result += ")";
    }

private:
    std::string result;
};


TEST(ParserTest, TestLiteralExpression) {
    Lexer lexer("123");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::shared_ptr<Expr> expr = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(expr), "123");
}

TEST(ParserTest, TestUnaryExpression) {
    Lexer lexer("!true");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::shared_ptr<Expr> expr = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(expr), "(! true)");
}

TEST(ParserTest, TestGroupingExpression) {
    Lexer lexer("(123)");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::shared_ptr<Expr> expr = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(expr), "(group 123)");
}

TEST(ParserTest, TestBinaryExpression) {
    Lexer lexer("1 + 2 * 3");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::shared_ptr<Expr> expr = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(expr), "(+ 1 (* 2 3))");
}

TEST(ParserTest, TestBinaryExpressionWithPrecedence) {
    Lexer lexer("1 * 2 + 3");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::shared_ptr<Expr> expr = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(expr), "(+ (* 1 2) 3)");
}
