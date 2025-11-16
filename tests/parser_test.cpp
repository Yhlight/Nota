#include "gtest/gtest.h"
#include "parser.hpp"
#include "lexer.hpp"
#include "ast.hpp"
#include <string>
#include <iostream>
#include <list>

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
    }

    void visit(const std::shared_ptr<VarStmt>& stmt) override {
        result += "(var ";
        result += stmt->name.lexeme;
        if (stmt->initializer) {
            result += " = ";
            stmt->initializer->accept(*this);
        }
        result += ")";
    }

    void visit(const std::shared_ptr<BlockStmt>& stmt) override {
        result += "{ ";
        for (const auto& statement : stmt->statements) {
            statement->accept(*this);
            result += " ";
        }
        result += "}";
    }

    void visit(const std::shared_ptr<IfStmt>& stmt) override {
        result += "(if ";
        stmt->condition->accept(*this);
        result += " ";
        stmt->thenBranch->accept(*this);
        if (stmt->elseBranch) {
            result += " else ";
            stmt->elseBranch->accept(*this);
        }
        result += ")";
    }

private:
    std::string result;
};


TEST(ParserTest, TestIfStatement) {
    Lexer lexer("if (true) let a = 1 end");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(if true { (var a = 1) })");
}

TEST(ParserTest, TestIfElseStatement) {
    Lexer lexer("if (true) let a = 1 else let b = 2 end");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(if true { (var a = 1) } else { (var b = 2) })");
}

TEST(ParserTest, TestIfElseIfElseStatement) {
    Lexer lexer("if (true) let a = 1 else if (false) let b = 2 else let c = 3 end");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(if true { (var a = 1) } else (if false { (var b = 2) } else { (var c = 3) }))");
}
