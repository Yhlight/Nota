#include "NotaLambda.h"
#include "Return.h"

namespace nota {

NotaLambda::NotaLambda(ast::LambdaExpr *declaration,
                         std::shared_ptr<Environment> closure)
    : declaration(declaration), closure(closure) {}

int NotaLambda::arity() { return declaration->params.size(); }

Value NotaLambda::call(Interpreter &interpreter,
                           std::vector<Value> arguments) {
    auto environment = std::make_shared<Environment>(closure);
    for (int i = 0; i < declaration->params.size(); ++i) {
        environment->define(declaration->params[i].name.lexeme, arguments[i]);
    }

    try {
        if (std::holds_alternative<std::unique_ptr<ast::Expr>>(declaration->body)) {
            return interpreter.evaluate_in_environment(*std::get<std::unique_ptr<ast::Expr>>(declaration->body), environment);
        } else {
            interpreter.execute_block(
                ((ast::BlockStmt *)std::get<std::unique_ptr<ast::Stmt>>(declaration->body).get())->statements,
                environment);
        }
    } catch (Return returnValue) {
        return returnValue.value;
    }

    return std::monostate();
}

} // namespace nota
