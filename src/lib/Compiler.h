#ifndef NOTA_COMPILER_H
#define NOTA_COMPILER_H

#include "Expr.h"
#include "Chunk.h"
#include <memory>

namespace nota {

class Compiler : public ExprVisitor {
public:
    Chunk compile(const std::unique_ptr<Expr>& expr);

    std::any visitBinaryExpr(const Binary& expr) override;
    std::any visitGroupingExpr(const Grouping& expr) override;
    std::any visitLiteralExpr(const Literal& expr) override;
    std::any visitUnaryExpr(const Unary& expr) override;

private:
    Chunk chunk;
};

} // namespace nota

#endif // NOTA_COMPILER_H
