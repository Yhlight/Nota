#ifndef NOTA_EXPR_H
#define NOTA_EXPR_H

#include "../Token.h"
#include <memory>
#include <vector>
#include <any>

namespace nota::ast {

    // Forward declaration for the visitor
    class ExprVisitor;

    class Expr {
    public:
        virtual ~Expr() = default;
        virtual std::any accept(ExprVisitor& visitor) = 0;
    };

} // namespace nota::ast

#endif //NOTA_EXPR_H
