#include "PrettyPrinter.h"

std::string PrettyPrinter::print(Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any PrettyPrinter::visit(BinaryExpr& expr) {
    return parenthesize(expr.op.lexeme, *expr.left, *expr.right);
}

std::any PrettyPrinter::visit(UnaryExpr& expr) {
    return parenthesize(expr.op.lexeme, *expr.right);
}

std::any PrettyPrinter::visit(LiteralExpr& expr) {
    return expr.value.lexeme;
}

std::any PrettyPrinter::visit(VariableExpr& expr) {
    return expr.name.lexeme;
}

std::any PrettyPrinter::visit(VarDeclStmt& stmt) {
    return "";
}

std::any PrettyPrinter::visit(PrintStmt& stmt) {
    return "";
}

std::any PrettyPrinter::visit(ExprStmt& stmt) {
    return "";
}

std::string PrettyPrinter::parenthesize(const std::string& name, Expr& expr) {
    return "(" + name + " " + std::any_cast<std::string>(expr.accept(*this)) + ")";
}

std::string PrettyPrinter::parenthesize(const std::string& name, Expr& left, Expr& right) {
    return "(" + name + " " + std::any_cast<std::string>(left.accept(*this)) + " " + std::any_cast<std::string>(right.accept(*this)) + ")";
}
