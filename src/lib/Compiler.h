#ifndef NOTA_COMPILER_H
#define NOTA_COMPILER_H

#include "Expr.h"
#include "Stmt.h"
#include "Chunk.h"
#include <memory>
#include <vector>

namespace nota {

class Compiler : public ExprVisitor, public StmtVisitor {
public:
    Chunk compile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitBinaryExpr(const Binary& expr) override;
    std::any visitGroupingExpr(const Grouping& expr) override;
    std::any visitLiteralExpr(const Literal& expr) override;
    std::any visitUnaryExpr(const Unary& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;

    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitVarDeclStmt(const VarDeclStmt& stmt) override;

private:
    void emitByte(uint8_t byte, int line);
    uint8_t makeConstant(Value value);
    void emitConstant(Value value, int line);

    Chunk chunk;
};

} // namespace nota

#endif // NOTA_COMPILER_H
