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
                 std::shared_ptr<Environment> closure, bool is_initializer);

    int arity() override;
    Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
    NotaFunction *bind(class NotaInstance *instance);

  private:
    ast::FuncDeclStmt *declaration;
    std::shared_ptr<Environment> closure;
    bool is_initializer;
};

} // namespace nota

#endif // NOTA_FUNCTION_H
