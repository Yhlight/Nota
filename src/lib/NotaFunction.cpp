#include "NotaFunction.h"
#include "Interpreter.h"
#include "Environment.h"
#include <vector>
#include <any>

NotaFunction::NotaFunction(std::shared_ptr<const FunctionStmt> declaration, std::shared_ptr<Environment> closure)
    : declaration(std::move(declaration)), closure(std::move(closure)) {}

NotaFunction::~NotaFunction() {}

int NotaFunction::arity() {
    return declaration->params.size();
}

std::any NotaFunction::call(Interpreter& interpreter, std::vector<std::any> arguments) {
    auto environment = std::make_shared<Environment>(closure);
    for (int i = 0; i < declaration->params.size(); ++i) {
        environment->define(declaration->params[i].lexeme, arguments[i]);
    }

    try {
        interpreter.executeBlock(declaration->body, environment);
    } catch (const Interpreter::Return& returnValue) {
        return returnValue.value;
    }

    return std::any{};
}
