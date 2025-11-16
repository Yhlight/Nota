#include "Environment.h"
#include "RuntimeError.h"

Environment::Environment() : enclosing(nullptr) {}

Environment::Environment(std::shared_ptr<Environment> enclosing) : enclosing(enclosing) {}


void Environment::define(const std::string& name, const std::any& value) {
    values[name] = value;
}

std::any Environment::get(const Token& name) {
    auto it = values.find(name.lexeme);
    if (it != values.end()) {
        return it->second;
    }

    if (enclosing != nullptr) return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, const std::any& value) {
    auto it = values.find(name.lexeme);
    if (it != values.end()) {
        it->second = value;
        return;
    }

    if (enclosing != nullptr) {
        enclosing->assign(name, value);
        return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
