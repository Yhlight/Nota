#ifndef NOTA_LAMBDA_H
#define NOTA_LAMBDA_H

#include "AST/Expr.h"
#include "Environment.h"
#include "Interpreter.h"
#include "Value.h"
#include <memory>
#include <vector>

namespace nota {

class NotaLambda : public Callable {
  public:
    NotaLambda(ast::LambdaExpr *declaration,
                 std::shared_ptr<Environment> closure);

    int arity() override;
    Value call(Interpreter &interpreter, std::vector<Value> arguments) override;

  private:
    ast::LambdaExpr *declaration;
    std::shared_ptr<Environment> closure;
};

} // namespace nota

#endif // NOTA_LAMBDA_H
