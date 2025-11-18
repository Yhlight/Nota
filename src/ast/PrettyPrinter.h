#pragma once

#include "Visitor.h"
#include "Expr.h"
#include <string>
#include <any>

class PrettyPrinter : public Visitor {
public:
    std::string print(Expr& expr);

    std::any visit(BinaryExpr& expr) override;
    std::any visit(UnaryExpr& expr) override;
    std::any visit(LiteralExpr& expr) override;

private:
    std::string parenthesize(const std::string& name, Expr& expr);
    std::string parenthesize(const std::string& name, Expr& left, Expr& right);
};
