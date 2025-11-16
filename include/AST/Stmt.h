#ifndef NOTA_AST_STMT_H
#define NOTA_AST_STMT_H

#include "../Token.h"
#include "Expr.h"
#include <memory>
#include <optional>

namespace nota {
namespace ast {

    template<typename R>
    class StmtVisitor {
    public:
        virtual R visit(class VarDeclStmt& stmt) = 0;
    };

    class Stmt {
    public:
        virtual ~Stmt() = default;
        virtual void accept(StmtVisitor<void>& visitor) = 0;
    };

    class VarDeclStmt : public Stmt {
    public:
        VarDeclStmt(Token name, std::optional<Token> type, std::unique_ptr<Expr> initializer)
            : name(name), type(type), initializer(std::move(initializer)) {}

        void accept(StmtVisitor<void>& visitor) override {
            visitor.visit(*this);
        }

        Token name;
        std::optional<Token> type;
        std::unique_ptr<Expr> initializer;
    };

} // namespace ast
} // namespace nota

#endif // NOTA_AST_STMT_H
