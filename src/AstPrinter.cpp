#include "AstPrinter.h"
#include <sstream>

std::string AstPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::string AstPrinter::print(const Stmt& stmt) {
    return std::any_cast<std::string>(stmt.accept(*this));
}

std::any AstPrinter::visitBinaryExpr(const Binary& expr) {
    return parenthesize(expr.op.lexeme, std::vector<const Expr*>{expr.left.get(), expr.right.get()});
}

std::any AstPrinter::visitGroupingExpr(const Grouping& expr) {
    return parenthesize("group", std::vector<const Expr*>{expr.expression.get()});
}

std::any AstPrinter::visitLiteralExpr(const Literal& expr) {
    if (expr.value.type() == typeid(std::monostate)) {
        return std::string("nil");
    }
    if (expr.value.type() == typeid(double)) {
        std::string str = std::to_string(std::any_cast<double>(expr.value));
        // Remove trailing zeros
        str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
        if(str.back() == '.') {
            str.pop_back();
        }
        return str;
    }
    if (expr.value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(expr.value);
    }
    if (expr.value.type() == typeid(bool)) {
        return std::any_cast<bool>(expr.value) ? std::string("true") : std::string("false");
    }
    return std::string("error");
}

std::any AstPrinter::visitUnaryExpr(const Unary& expr) {
    return parenthesize(expr.op.lexeme, std::vector<const Expr*>{expr.right.get()});
}

std::any AstPrinter::visitVariableExpr(const Variable& expr) {
    return expr.name.lexeme;
}

std::any AstPrinter::visitAssignExpr(const Assign& expr) {
    return parenthesize("= " + expr.name.lexeme, std::vector<const Expr*>{expr.value.get()});
}

std::any AstPrinter::visitBlockStmt(const Block& stmt) {
    std::vector<const Stmt*> stmts;
    for(const auto& s : stmt.statements) {
        stmts.push_back(s.get());
    }
    return parenthesize("block", stmts);
}

std::any AstPrinter::visitExpressionStmt(const Expression& stmt) {
    return parenthesize(";", std::vector<const Expr*>{stmt.expression.get()});
}

std::any AstPrinter::visitIfStmt(const If& stmt) {
    if (stmt.elseBranch != nullptr) {
        return parenthesize("if-else", std::vector<const Expr*>{stmt.condition.get()}, std::vector<const Stmt*>{stmt.thenBranch.get(), stmt.elseBranch.get()});
    }
    return parenthesize("if", std::vector<const Expr*>{stmt.condition.get()}, std::vector<const Stmt*>{stmt.thenBranch.get()});
}


std::any AstPrinter::visitPrintStmt(const Print& stmt) {
    return parenthesize("print", std::vector<const Expr*>{stmt.expression.get()});
}

std::any AstPrinter::visitVarStmt(const Var& stmt) {
    if (stmt.initializer != nullptr) {
        return parenthesize("var " + stmt.name.lexeme, std::vector<const Expr*>{stmt.initializer.get()});
    }
    return parenthesize("var " + stmt.name.lexeme, std::vector<const Expr*>{});
}

std::any AstPrinter::visitWhileStmt(const While& stmt) {
    return parenthesize("while", std::vector<const Expr*>{stmt.condition.get()}, std::vector<const Stmt*>{stmt.body.get()});
}


std::string AstPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& expr : exprs) {
        ss << " ";
        ss << std::any_cast<std::string>(expr->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::string AstPrinter::parenthesize(const std::string& name, const std::vector<const Stmt*>& stmts) {
    std::stringstream ss;
    ss << "(" << name;
    for (const auto& stmt : stmts) {
        ss << " ";
        ss << std::any_cast<std::string>(stmt->accept(*this));
    }
    ss << ")";
    return ss.str();
}

std::string AstPrinter::parenthesize(const std::string& name, const std::vector<const Expr*>& exprs, const std::vector<const Stmt*>& stmts) {
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
