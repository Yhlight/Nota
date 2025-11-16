#include <gtest/gtest.h>
#include "../src/parser.hpp"
#include "../src/ast.hpp"
#include <iostream>

using namespace nota;

class AstPrinter : public ExprVisitor, public StmtVisitor {
public:
    std::string print(const std::vector<std::unique_ptr<Stmt>>& statements) {
        std::string result;
        for (const auto& stmt : statements) {
            result += stmt->accept(*this) + "\n";
        }
        return result;
    }

    std::string visit(const BinaryExpr& expr) override {
        return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
    }

    std::string visit(const GroupingExpr& expr) override {
        return parenthesize("group", {expr.expression.get()});
    }

    std::string visit(const LiteralExpr& expr) override {
        return expr.value;
    }

    std::string visit(const UnaryExpr& expr) override {
        return parenthesize(expr.op.lexeme, {expr.right.get()});
    }

    std::string visit(const VariableExpr& expr) override {
        return expr.name.lexeme;
    }

    std::string visit(const AssignExpr& expr) override {
        return parenthesize("= " + expr.name.lexeme, {expr.value.get()});
    }

    std::string visit(const CallExpr& expr) override {
        std::vector<Expr*> args;
        for(const auto& arg : expr.arguments) {
            args.push_back(arg.get());
        }
        return parenthesize("call " + expr.callee->accept(*this), args);
    }

    std::string visit(const LambdaExpr& expr) override {
        std::string result = "lambda (";
        for (size_t i = 0; i < expr.params.size(); ++i) {
            result += expr.params[i].name.lexeme;
            if (expr.params[i].type) {
                result += ": " + expr.params[i].type->accept(*this);
            }
            if (i < expr.params.size() - 1) {
                result += ", ";
            }
        }
        result += ") => " + expr.body->accept(*this);
        return result;
    }

    std::string visit(const TypeExpr& expr) override {
        return expr.name.lexeme;
    }

    std::string visit(const ExpressionStmt& stmt) override {
        return stmt.expression->accept(*this) + ";";
    }

    std::string visit(const VarStmt& stmt) override {
        std::string type = stmt.is_mutable ? "mut" : "let";
        std::string result = type + " " + stmt.name.lexeme;
        if (stmt.type) {
            result += ": " + stmt.type->accept(*this);
        }
        if (stmt.initializer) {
            result += " = " + stmt.initializer->accept(*this);
        }
        return result + ";";
    }

    std::string visit(const BlockStmt& stmt) override {
        std::string result = "{\n";
        for (const auto& s : stmt.statements) {
            result += s->accept(*this) + "\n";
        }
        result += "}";
        return result;
    }

    std::string visit(const IfStmt& stmt) override {
        std::string result = "if " + stmt.condition->accept(*this) + " " + stmt.then_branch->accept(*this);
        if (stmt.else_branch) {
            result += " else " + stmt.else_branch->accept(*this);
        }
        return result;
    }

    std::string visit(const WhileStmt& stmt) override {
        return "while " + stmt.condition->accept(*this) + " " + stmt.body->accept(*this);
    }

    std::string visit(const FunctionStmt& stmt) override {
        std::string result = "func " + stmt.name.lexeme + "(";
        for (size_t i = 0; i < stmt.params.size(); ++i) {
            result += stmt.params[i].name.lexeme;
            if (stmt.params[i].type) {
                result += ": " + stmt.params[i].type->accept(*this);
            }
            if (i < stmt.params.size() - 1) {
                result += ", ";
            }
        }
        result += ")";
        if (stmt.return_type) {
            result += ": " + stmt.return_type->accept(*this);
        }
        result += " " + stmt.body->accept(*this);
        return result;
    }

    std::string visit(const ReturnStmt& stmt) override {
        return "return"; // Simplified
    }

    std::string visit(const DoWhileStmt& stmt) override {
        return "do " + stmt.body->accept(*this) + " while " + stmt.condition->accept(*this);
    }


private:
    std::string parenthesize(const std::string& name, const std::vector<Expr*>& exprs) {
        std::string result = "(" + name;
        for (const auto& expr : exprs) {
            result += " " + expr->accept(*this);
        }
        result += ")";
        return result;
    }
};

TEST(ParserTest, VarDeclaration) {
    std::string source = "let x = 10";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "let x = 10;\n");
}

TEST(ParserTest, VarDeclarationWithType) {
    std::string source = "let x: int = 10";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "let x: int = 10;\n");
}

TEST(ParserTest, FunctionDeclarationWithType) {
    std::string source = "func add(a: int, b: int): int a + b end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "func add(a: int, b: int): int (+ a b);\n");
}

TEST(ParserTest, DoWhileLoop) {
    std::string source = "do x = x + 1 while x < 10";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "do (= x (+ x 1)); while (< x 10)\n");
}
