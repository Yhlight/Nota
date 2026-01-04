#ifndef NOTA_STMT_H
#define NOTA_STMT_H

#include "Token.h"
#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

namespace nota::ast {

    class ComponentStmt; // Forward declaration

    class StmtVisitor {
    public:
        virtual ~StmtVisitor() = default;
        virtual std::any visitComponentStmt(ComponentStmt& stmt) = 0;
    };

    class Stmt {
    public:
        virtual ~Stmt() = default;
        virtual std::any accept(StmtVisitor& visitor) = 0;
    };

    class ComponentStmt : public Stmt {
    public:
        const Token name;
        const std::vector<std::unique_ptr<Stmt>> body;

        ComponentStmt(Token name, std::vector<std::unique_ptr<Stmt>> body)
            : name(std::move(name)), body(std::move(body)) {}

        std::any accept(StmtVisitor& visitor) override {
            return visitor.visitComponentStmt(*this);
        }
    };

} // namespace nota::ast

#endif //NOTA_STMT_H
