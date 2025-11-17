#include "NotaFunction.h"
#include "Return.h"

namespace nota {

NotaFunction::NotaFunction(ast::FuncDeclStmt *declaration,
                           std::shared_ptr<Environment> closure)
    : declaration(declaration), closure(closure) {}

int NotaFunction::arity() { return declaration->params.size(); }

Value NotaFunction::call(Interpreter &interpreter,
                         std::vector<Value> arguments) {
    auto environment = std::make_shared<Environment>(closure);
    for (int i = 0; i < declaration->params.size(); ++i) {
        environment->define(declaration->params[i].name.lexeme, arguments[i]);
    }

    try {
        interpreter.execute_block(
            ((ast::BlockStmt *)declaration->body.get())->statements,
            environment);
    } catch (Return returnValue) {
        return returnValue.value;
    }

    return std::monostate();
}

} // namespace nota
