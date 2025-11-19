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
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                oss << "none";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, expr.value);
    }

    void visit(const Unary& expr) override {
        parenthesize(expr.op.lexeme, {expr.right});
    }

    void visit(const Variable& expr) override {
        oss << expr.name.lexeme;
    }

    void visit(const Assign& expr) override {
        oss << "(" << expr.name.lexeme << " = ";
        expr.value->accept(*this);
        oss << ")";
    }

    void visit(const Postfix& expr) override {
        oss << "(";
        expr.left->accept(*this);
        oss << expr.op.lexeme << ")";
    }

    void visit(const BlockStmt& stmt) override {
        oss << "{\n";
        for (const auto& s : stmt.statements) {
            s->accept(*this);
        }
        oss << "}\n";
    }

    void visit(const IfStmt& stmt) override {
        oss << "if (";
        stmt.condition->accept(*this);
        oss << ") ";
        stmt.thenBranch->accept(*this);
        if (stmt.elseBranch) {
            oss << "else ";
            stmt.elseBranch->accept(*this);
        }
    }

    void visit(const WhileStmt& stmt) override {
        oss << "while (";
        stmt.condition->accept(*this);
        oss << ") ";
        stmt.body->accept(*this);
    }

    void visit(const ForStmt& stmt) override {
        oss << "for (";
        if (stmt.initializer) {
            stmt.initializer->accept(*this);
        }
        oss << "; ";
        if (stmt.condition) {
            stmt.condition->accept(*this);
        }
        oss << "; ";
        if (stmt.increment) {
            stmt.increment->accept(*this);
        }
        oss << ") ";
        stmt.body->accept(*this);
    }

    void visit(const DoWhileStmt& stmt) override {
        oss << "do ";
        stmt.body->accept(*this);
        oss << "while (";
        stmt.condition->accept(*this);
        oss << ")";
    }

    void visit(const Call& expr) override {
        oss << "call(";
        expr.callee->accept(*this);
        oss << ", args=";
        for (const auto& arg : expr.arguments) {
            arg->accept(*this);
            oss << " ";
        }
        oss << ")";
    }

    void visit(const FunctionStmt& stmt) override {
        oss << "func " << stmt.name.lexeme << "(";
        for (const auto& param : stmt.params) {
            oss << param.lexeme << " ";
        }
        oss << ") {\n";
        for (const auto& s : stmt.body) {
            s->accept(*this);
        }
        oss << "}\n";
    }

    void visit(const ReturnStmt& stmt) override {
        oss << "return ";
        if (stmt.value) {
            stmt.value->accept(*this);
        }
        oss << "\n";
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

TEST(ParserTest, ParsesIfStatement) {
    std::string expected = "if ((> 1 0)) {\nlet a = 2\n}\n";
    EXPECT_EQ(parseAndPrint("if 1 > 0\nlet a = 2\nend\n"), expected);
}

TEST(ParserTest, ParsesWhileStatement) {
    std::string expected = "while ((< a 5)) {\n(a = (+ a 1))\n}\n";
    EXPECT_EQ(parseAndPrint("while a < 5\na = a + 1\nend\n"), expected);
}

TEST(ParserTest, ParsesForStatement) {
    std::string expected = "for (mut i = 0\n; (< i 5); (i++)) {\n(a = (+ a 1))\n}\n";
    EXPECT_EQ(parseAndPrint("for mut i = 0; i < 5; i++\na = a + 1\nend\n"), expected);
}

TEST(ParserTest, ParsesFunctionDeclaration) {
    std::string expected = "func add(a b ) {\nreturn (+ a b)\n}\n";
    EXPECT_EQ(parseAndPrint("func add(a, b)\nreturn a + b\nend\n"), expected);
}

TEST(ParserTest, ParsesFunctionCall) {
    std::string expected = "call(add, args=(+ 1 2) )\n";
    EXPECT_EQ(parseAndPrint("add(1 + 2)\n"), expected);
}
