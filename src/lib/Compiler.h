#ifndef NOTA_COMPILER_H
#define NOTA_COMPILER_H

#include "Expr.h"
#include "Stmt.h"
#include "Chunk.h"
#include "SymbolTable.h"
#include "Object.h"
#include <memory>
#include <vector>

namespace nota {

class Compiler : public ExprVisitor, public StmtVisitor {
public:
    Compiler();
    Compiler(Compiler* enclosing);
    std::shared_ptr<NotaFunction> compile(const std::vector<std::unique_ptr<Stmt>>& statements);

    std::any visitBinaryExpr(const Binary& expr) override;
    std::any visitGroupingExpr(const Grouping& expr) override;
    std::any visitLiteralExpr(const Literal& expr) override;
    std::any visitUnaryExpr(const Unary& expr) override;
    std::any visitVariableExpr(const VariableExpr& expr) override;
    std::any visitAssignExpr(const AssignExpr& expr) override;
    std::any visitCallExpr(const CallExpr& expr) override;

    void visitExpressionStmt(const ExpressionStmt& stmt) override;
    void visitVarDeclStmt(const VarDeclStmt& stmt) override;
    void visitBlockStmt(const BlockStmt& stmt) override;
    void visitIfStmt(const IfStmt& stmt) override;
    void visitWhileStmt(const WhileStmt& stmt) override;
    void visitForStmt(const ForStmt& stmt) override;
    void visitFunctionStmt(const FunctionStmt& stmt) override;
    void visitReturnStmt(const ReturnStmt& stmt) override;

private:
    void emitByte(uint8_t byte, int line);
    void emitBytes(uint8_t byte1, uint8_t byte2, int line);
    int emitJump(uint8_t instruction, int line);
    void patchJump(int offset);
    void emitLoop(int loopStart, int line);
    void declareVariable(Token name);
    uint8_t parseVariable(const std::string& errorMessage, int line);
    void defineVariable(uint8_t global, int line);
    void emitReturn();

    uint8_t makeConstant(Value value);
    void emitConstant(Value value, int line);

    void beginScope();
    void endScope();

    Compiler* enclosing;
    std::shared_ptr<NotaFunction> function;
    SymbolTable symbolTable;
};

} // namespace nota

#endif // NOTA_COMPILER_H
