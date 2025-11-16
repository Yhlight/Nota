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
            result += expr.params[i].lexeme;
            if (i < expr.params.size() - 1) {
                result += ", ";
            }
        }
        result += ") => " + expr.body->accept(*this);
        return result;
    }

    std::string visit(const ExpressionStmt& stmt) override {
        return stmt.expression->accept(*this) + ";";
    }

    std::string visit(const VarStmt& stmt) override {
        std::string type = stmt.is_mutable ? "mut" : "let";
        if (stmt.initializer) {
            return type + " " + stmt.name.lexeme + " = " + stmt.initializer->accept(*this) + ";";
        }
        return type + " " + stmt.name.lexeme + ";";
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
            result += stmt.params[i].lexeme;
            if (i < stmt.params.size() - 1) {
                result += ", ";
            }
        }
        result += ") " + stmt.body->accept(*this);
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

TEST(ParserTest, FunctionCall) {
    std::string source = "my_function(1, 2)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "(call my_function 1 2);\n");
}

TEST(ParserTest, ForLoop) {
    std::string source = "for let i = 0; i < 10; i = i + 1 i = i + 1 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    EXPECT_EQ(statements.size(), 1);

    AstPrinter printer;
    std::string result = printer.print(statements);
    EXPECT_EQ(result, "{\nlet i = 0;\nwhile (< i 10) {\n(= i (+ i 1));\n(= i (+ i 1));\n}\n}\n");
}

TEST(ParserTest, InvalidAssignment) {
    std::string source = "10 = x";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);

    // The parser should detect the error and recover, resulting in no valid statements.
    auto statements = parser.parse();
    EXPECT_EQ(statements.size(), 0);
}

TEST(ParserTest, FunctionDeclaration) {
    std::string source = "func my_func(a, b) a + b end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "func my_func(a, b) (+ a b);\n");
}

TEST(ParserTest, LambdaExpression) {
    std::string source = "let add = (a, b) => a + b";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "let add = lambda (a, b) => (+ a b);\n");
}

TEST(ParserTest, LambdaExpressionNoArgs) {
    std::string source = "let get_42 = () => 42";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "let get_42 = lambda () => 42;\n");
}

TEST(ParserTest, DoWhileLoop) {
    std::string source = "do x = x + 1 while x < 10 end";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scan_tokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    AstPrinter printer;
    std::string result = printer.print(statements);

    EXPECT_EQ(statements.size(), 1);
    EXPECT_EQ(result, "do (= x (+ x 1)); while (< x 10)\n");
}
