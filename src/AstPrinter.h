#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "Expr.h"
#include <string>
#include <vector>

class AstPrinter : public ExprVisitor<std::any> {
public:
    std::string print(const Expr& expr);

    std::any visitBinaryExpr(const Binary& expr) override;
    std::any visitGroupingExpr(const Grouping& expr) override;
    std::any visitLiteralExpr(const Literal& expr) override;
    std::any visitUnaryExpr(const Unary& expr) override;

private:
    std::string parenthesize(const std::string& name, const std::vector<const Expr*>& exprs);
};

#endif // AST_PRINTER_H
