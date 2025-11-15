#ifndef NOTA_COMPILER_HPP
#define NOTA_COMPILER_HPP

#include "../ast.hpp"
#include "chunk.hpp"
#include <memory>

namespace nota {

    class Compiler : public ExprVisitor, public StmtVisitor {
    public:
        Compiler();
        std::unique_ptr<Chunk> compile(const std::vector<std::unique_ptr<Stmt>>& statements);

        // Expression visitors
        std::any visit(const BinaryExpr& expr) override;
        std::any visit(const UnaryExpr& expr) override;
        std::any visit(const LiteralExpr& expr) override;
        std::any visit(const GroupingExpr& expr) override;
        std::any visit(const VariableExpr& expr) override;
        std::any visit(const AssignExpr& expr) override;

        // Statement visitors
        void visit(const ExpressionStmt& stmt) override;
        void visit(const VarDeclStmt& stmt) override;
        void visit(const BlockStmt& stmt) override;

    private:
        void emitByte(uint8_t byte);
        void emitBytes(uint8_t byte1, uint8_t byte2);
        void emitReturn();
        void emitConstant(const LiteralValue& value);
        uint8_t identifierConstant(const Token& name);

        std::unique_ptr<Chunk> chunk;
    };

} // namespace nota

#endif // NOTA_COMPILER_HPP
