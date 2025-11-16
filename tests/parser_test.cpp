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

    void visit(const std::shared_ptr<PostfixExpr>& expr) override {
        result += "(";
        result += expr->op.lexeme;
        result += " ";
        expr->left->accept(*this);
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

    void visit(const std::shared_ptr<WhileStmt>& stmt) override {
        result += "(while ";
        stmt->condition->accept(*this);
        result += " ";
        stmt->body->accept(*this);
        result += ")";
    }

    void visit(const std::shared_ptr<DoWhileStmt>& stmt) override {
        result += "(do-while ";
        stmt->body->accept(*this);
        result += " ";
        stmt->condition->accept(*this);
        result += ")";
    }

    void visit(const std::shared_ptr<ForStmt>& stmt) override {
        result += "(for ";
        stmt->initializer->accept(*this);
        result += " ";
        stmt->condition->accept(*this);
        result += " ";
        stmt->increment->accept(*this);
        result += " ";
        stmt->body->accept(*this);
        result += ")";
    }

private:
    std::string result;
};


TEST(ParserTest, TestForStatement) {
    Lexer lexer("for (let i = 0; i < 10; i++) let a = 1 end");
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> stmts = parser.parse();
    ASTPrinter printer;
    ASSERT_EQ(printer.print(stmts[0]), "(for (var i = 0) (< i 10) (++ i) { (var a = 1) })");
}
