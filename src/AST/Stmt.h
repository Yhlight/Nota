#ifndef NOTA_STMT_H
#define NOTA_STMT_H

#include "Token.h"
#include "Expr.h"
#include <memory>
#include <vector>
#include <any>

namespace nota::ast {

    // Forward declarations
    class ComponentStmt;
    class PropertyStmt;

    class StmtVisitor {
    public:
        virtual ~StmtVisitor() = default;
        virtual std::any visitComponentStmt(ComponentStmt& stmt) = 0;
        virtual std::any visitPropertyStmt(PropertyStmt& stmt) = 0;
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

    class PropertyStmt : public Stmt {
    public:
        const Token name;
        const std::unique_ptr<Expr> value;

        PropertyStmt(Token name, std::unique_ptr<Expr> value)
            : name(std::move(name)), value(std::move(value)) {}

        std::any accept(StmtVisitor& visitor) override {
            return visitor.visitPropertyStmt(*this);
        }
    };

} // namespace nota::ast

#endif //NOTA_STMT_H
