#include "vendor/doctest.h"
#include "Parser.h"
#include "AST.h"
#include "Lexer.h"

class ASTPrinter : public nota::ExprVisitor {
public:
    std::string print(std::shared_ptr<nota::Expr> expr) {
        expr->accept(*this);
        return result_;
    }

    void visit(const std::shared_ptr<nota::Binary>& expr) override {
        result_ = "(" + expr->op.lexeme + " " + print(expr->left) + " " + print(expr->right) + ")";
    }

    void visit(const std::shared_ptr<nota::Grouping>& expr) override {
        result_ = "(group " + print(expr->expression) + ")";
    }

    void visit(const std::shared_ptr<nota::Literal>& expr) override {
        if (std::holds_alternative<int>(expr->value)) {
            result_ = std::to_string(std::get<int>(expr->value));
        } else if (std::holds_alternative<double>(expr->value)) {
            result_ = std::to_string(std::get<double>(expr->value));
        } else if (std::holds_alternative<std::string>(expr->value)) {
            result_ = std::get<std::string>(expr->value);
        } else {
            result_ = "nil";
        }
    }

    void visit(const std::shared_ptr<nota::Unary>& expr) override {
        result_ = "(" + expr->op.lexeme + " " + print(expr->right) + ")";
    }

private:
    std::string result_;
};


TEST_CASE("Parser parses a numeric literal") {
    nota::Lexer lexer("123");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::shared_ptr<nota::Expr> expr = parser.parse();

    ASTPrinter printer;
    CHECK(printer.print(expr) == "123");
}

TEST_CASE("Parser parses a unary expression") {
    nota::Lexer lexer("-123");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::shared_ptr<nota::Expr> expr = parser.parse();

    ASTPrinter printer;
    CHECK(printer.print(expr) == "(- 123)");
}

TEST_CASE("Parser parses a binary expression") {
    nota::Lexer lexer("1 + 2");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::shared_ptr<nota::Expr> expr = parser.parse();

    ASTPrinter printer;
    CHECK(printer.print(expr) == "(+ 1 2)");
}

TEST_CASE("Parser parses a grouping expression") {
    nota::Lexer lexer("(1 + 2)");
    std::vector<nota::Token> tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    std::shared_ptr<nota::Expr> expr = parser.parse();

    ASTPrinter printer;
    CHECK(printer.print(expr) == "(group (+ 1 2))");
}
