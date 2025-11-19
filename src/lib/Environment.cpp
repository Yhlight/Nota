#include "Environment.h"
#include <stdexcept>

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(std::move(enclosing)) {}

void Environment::define(const std::string& name, const std::any& value, bool isMutable) {
    values[name] = {value, isMutable};
}

void Environment::assign(const Token& name, const std::any& value) {
    if (values.count(name.lexeme)) {
        if (!values.at(name.lexeme).isMutable) {
            throw std::runtime_error("Cannot assign to immutable variable '" + name.lexeme + "'.");
        }
        values[name.lexeme].value = value;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

const std::any& Environment::get(const Token& name) {
    if (values.count(name.lexeme)) {
        return values.at(name.lexeme).value;
    }

    if (enclosing != nullptr) {
        return enclosing->get(name);
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}
