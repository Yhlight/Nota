#include "Environment.h"
#include "RuntimeError.h"

Environment::Environment() {}

void Environment::define(const std::string& name, const std::any& value) {
    values[name] = value;
}

std::any Environment::get(const Token& name) {
    auto it = values.find(name.lexeme);
    if (it != values.end()) {
        return it->second;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}
