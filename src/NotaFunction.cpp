#include "NotaFunction.h"
#include "Return.h"

namespace nota {

NotaFunction::NotaFunction(ast::FuncDeclStmt *declaration,
                           std::shared_ptr<Environment> closure, bool is_initializer)
    : declaration(declaration), closure(closure), is_initializer(is_initializer) {}

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

    if (is_initializer) {
        Token this_token{TokenType::This, "this", 0, 0};
        return closure->get(this_token);
    }

    return std::monostate();
}

NotaFunction *NotaFunction::bind(NotaInstance *instance) {
    auto environment = std::make_shared<Environment>(closure);
    environment->define("this", instance);
    return new NotaFunction(declaration, environment, is_initializer);
}

} // namespace nota
