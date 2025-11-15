#include "gtest/gtest.h"
#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <string>
#include <iostream>

// ASTPrinter implementation for testing
class ASTPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(std::shared_ptr<Expr> expr) {
        expr->accept(*this);
        return result;
    }

    std::string print(std::shared_ptr<Stmt> stmt) {
        stmt->accept(*this);
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

    void visit(const std::shared_ptr<ExpressionStmt>& stmt) override {
        stmt->expression->accept(*this);
        result += ";";
    }

    void visit(const std::shared_ptr<VarStmt>& stmt) override {
        result += "(var ";
        result += stmt->name.lexeme;
        if (stmt->initializer) {
            result += " = ";
            stmt->initializer->accept(*this);
        }
        result += ";)";
    }

private:
    std::string result;
};


TEST(ParserTest, TestLiteralExpression) {
    Lexer lexer("123;");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "123;");
}

TEST(ParserTest, TestUnaryExpression) {
    Lexer lexer("!true;");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(! true);");
}

TEST(ParserTest, TestGroupingExpression) {
    Lexer lexer("(123);");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(group 123);");
}

TEST(ParserTest, TestBinaryExpression) {
    Lexer lexer("1 + 2 * 3;");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(+ 1 (* 2 3));");
}

TEST(ParserTest, TestBinaryExpressionWithPrecedence) {
    Lexer lexer("1 * 2 + 3;");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(+ (* 1 2) 3);");
}

TEST(ParserTest, TestVariableDeclaration) {
    Lexer lexer("let a = 1;");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(var a = 1;)");
}

TEST(ParserTest, TestMutableVariableDeclaration) {
    Lexer lexer("mut a = 1;");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(var a = 1;)");
}
