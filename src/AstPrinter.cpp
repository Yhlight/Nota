#include "AstPrinter.h"
#include <sstream>

std::string AstPrinter::print(const Expr& expr) {
    return std::any_cast<std::string>(expr.accept(*this));
}

std::any AstPrinter::visitBinaryExpr(const Binary& expr) {
    return parenthesize(expr.op.lexeme, {expr.left.get(), expr.right.get()});
}

std::any AstPrinter::visitGroupingExpr(const Grouping& expr) {
    return parenthesize("group", {expr.expression.get()});
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
    return parenthesize(expr.op.lexeme, {expr.right.get()});
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
