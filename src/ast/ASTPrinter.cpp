#include "ast/ASTPrinter.h"
#include <sstream>
#include <vector>

std::string ASTPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::string ASTPrinter::print(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::any ASTPrinter::visit(const Binary& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()}, {});
}

std::any ASTPrinter::visit(const Grouping& expr) {
    return parenthesize("group", {expr.expression.get()}, {});
}

std::any ASTPrinter::visit(const Literal& expr) {
    if (expr.value.has_value()) {
        if (expr.value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(expr.value));
        }
        if (expr.value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(expr.value));
        }
        if (expr.value.type() == typeid(std::string)) {
            return std::any_cast<std::string>(expr.value);
        }
        if (expr.value.type() == typeid(bool)) {
            return std::string(std::any_cast<bool>(expr.value) ? "true" : "false");
        }
    }
    return std::string("nil");
}

std::any ASTPrinter::visit(const Unary& expr) {
    return parenthesize(expr.op.lexeme, {expr.right.get()}, {});
}

std::any ASTPrinter::visit(const Variable& expr) {
    return expr.name.lexeme;
}

std::any ASTPrinter::visit(const ExpressionStmt& stmt) {
    return parenthesize(";", {stmt.expression.get()}, {});
}

std::any ASTPrinter::visit(const VarStmt& stmt) {
    if (stmt.initializer) {
        return parenthesize("var " + stmt.name.lexeme, {stmt.initializer.get()}, {});
    }
    return "(var " + stmt.name.lexeme + ")";
}

std::any ASTPrinter::visit(const IfStmt& stmt) {
    if (stmt.elseBranch) {
        return parenthesize("if-else", {stmt.condition.get()}, {stmt.thenBranch.get(), stmt.elseBranch.get()});
    }
    return parenthesize("if", {stmt.condition.get()}, {stmt.thenBranch.get()});
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " ";
        ss << std::any_cast<std::string>(expr->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::string ASTPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs, const std::vector<const Stmt*>& stmts) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " ";
        ss << std::any_cast<std::string>(expr->accept(*this));
    }
    for (const auto& stmt : stmts) {
        ss << " ";
        ss << std::any_cast<std::string>(stmt->accept(*this));
    }
    ss << ")";
    return ss.str();
}
