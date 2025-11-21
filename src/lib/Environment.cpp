#include "Environment.h"
#include <stdexcept>

namespace nota {

void Environment::define(const std::string& name, const Value& value) {
    values_[name] = value;
}

Environment::Value Environment::get(const Token& name) {
    if (values_.find(name.lexeme) != values_.end()) {
        return values_.at(name.lexeme);
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, const Value& value) {
    if (values_.find(name.lexeme) != values_.end()) {
        values_[name.lexeme] = value;
        return;
    }

    throw std::runtime_error("Undefined variable '" + name.lexeme + "'.");
}

} // namespace nota
