#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include <string>
#include <sstream>

// An AST printer implementation to verify the structure of the parsed expressions.
class AstPrinter : public ExprVisitor {
public:
    std::string print(const std::shared_ptr<Expr>& expr) {
        if (!expr) return "null";
        expr->accept(*this);
        return oss.str();
    }

    void visit(const Binary& expr) override {
        parenthesize(expr.op.lexeme, {expr.left, expr.right});
    }

    void visit(const Grouping& expr) override {
        parenthesize("group", {expr.expression});
    }

    void visit(const Literal& expr) override {
        if (std::holds_alternative<std::monostate>(expr.value)) {
            oss << "none";
        } else if (std::holds_alternative<int>(expr.value)) {
            oss << std::get<int>(expr.value);
        } else if (std::holds_alternative<double>(expr.value)) {
            oss << std::get<double>(expr.value);
        } else if (std::holds_alternative<std::string>(expr.value)) {
            oss << std::get<std::string>(expr.value);
        } else if (std::holds_alternative<bool>(expr.value)) {
            oss << (std::get<bool>(expr.value) ? "true" : "false");
        }
    }

    void visit(const Unary& expr) override {
        parenthesize(expr.op.lexeme, {expr.right});
    }

private:
    void parenthesize(const std::string& name, const std::vector<std::shared_ptr<Expr>>& exprs) {
        oss << "(" << name;
        for (const auto& expr : exprs) {
            oss << " ";
            expr->accept(*this);
        }
        oss << ")";
    }

    std::stringstream oss;
};

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::shared_ptr<Expr> expr = parser.parse();
    AstPrinter printer;
    return printer.print(expr);
}

TEST(ParserTest, ParsesNumericLiteral) {
    EXPECT_EQ(parseAndPrint("123"), "123");
}

TEST(ParserTest, ParsesSimpleBinaryExpression) {
    EXPECT_EQ(parseAndPrint("1 + 2"), "(+ 1 2)");
}

TEST(ParserTest, ParsesUnaryExpression) {
    EXPECT_EQ(parseAndPrint("-1"), "(- 1)");
}

TEST(ParserTest, HandlesPrecedence) {
    EXPECT_EQ(parseAndPrint("1 + 2 * 3"), "(+ 1 (* 2 3))");
    EXPECT_EQ(parseAndPrint("(1 + 2) * 3"), "(* (group (+ 1 2)) 3)");
}
