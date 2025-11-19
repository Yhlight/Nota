#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include <string>
#include <sstream>

// An AST printer implementation to verify the structure of the parsed expressions.
class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::shared_ptr<Stmt>>& stmts) {
        oss.str(""); // Clear the stream
        for (const auto& stmt : stmts) {
            if (stmt) {
                stmt->accept(*this);
            }
        }
        return oss.str();
    }

    void visit(const ExpressionStmt& stmt) override {
        stmt.expression->accept(*this);
        oss << "\n";
    }

    void visit(const VarStmt& stmt) override {
        oss << (stmt.isMutable ? "mut " : "let ") << stmt.name.lexeme;
        if (stmt.initializer) {
            oss << " = ";
            stmt.initializer->accept(*this);
        }
        oss << "\n";
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
    auto stmts = parser.parse();
    AstPrinter printer;
    return printer.print(stmts);
}

TEST(ParserTest, ParsesVarDeclaration) {
    EXPECT_EQ(parseAndPrint("let a = 1\n"), "let a = 1\n");
    EXPECT_EQ(parseAndPrint("mut b = \"hello\"\n"), "mut b = hello\n");
}

TEST(ParserTest, ParsesExpressionStatement) {
    EXPECT_EQ(parseAndPrint("1 + 2\n"), "(+ 1 2)\n");
}
