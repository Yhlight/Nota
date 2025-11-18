#pragma once

#include "AST.h"
#include <string>

// Note: This class now implements ExprVisitor<std::any> to match the AST's accept methods.
// The visit methods will return std::any containing a std::string.
class ASTPrinter : public ExprVisitor<std::any> {
public:
    std::string print(const Expr& expr);
    std::any visit(const Binary& expr) override;
    std::any visit(const Grouping& expr) override;
    std::any visit(const Literal& expr) override;
    std::any visit(const Unary& expr) override;
    std::any visit(const Variable& expr) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
};
