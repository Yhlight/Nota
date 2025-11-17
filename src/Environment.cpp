#include "Environment.h"
#include <stdexcept>

namespace nota {

Environment::Environment(std::shared_ptr<Environment> enclosing)
    : enclosing(enclosing) {}

void Environment::define(const std::string &name, Value value) {
    values[name] = value;
}

Value Environment::get(const Token &name) {
    if (values.count(name.lexeme)) {
        return values[name.lexeme];
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token &name, Value value) {
    if (values.count(name.lexeme)) {
        values[name.lexeme] = value;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

} // namespace nota
