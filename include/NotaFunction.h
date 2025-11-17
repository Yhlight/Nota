#ifndef NOTA_FUNCTION_H
#define NOTA_FUNCTION_H

#include "AST/Stmt.h"
#include "Environment.h"
#include "Interpreter.h"
#include "Value.h"
#include <memory>
#include <vector>

namespace nota {

class NotaFunction : public Callable {
  public:
    NotaFunction(ast::FuncDeclStmt *declaration,
                 std::shared_ptr<Environment> closure);

    int arity() override;
    Value call(Interpreter &interpreter, std::vector<Value> arguments) override;

  private:
    ast::FuncDeclStmt *declaration;
    std::shared_ptr<Environment> closure;
};

} // namespace nota

#endif // NOTA_FUNCTION_H
