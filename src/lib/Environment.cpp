#include "Environment.h"
#include "Interpreter.h"
#include <stdexcept>
#include "AST.h"

namespace nota {

void Environment::define(const std::string& name, const Value& value, bool is_mutable) {
    values_[name] = {value, is_mutable};
}

Value Environment::get(const Token& name) {
    if (values_.find(name.lexeme) != values_.end()) {
        return values_.at(name.lexeme).value;
    }

    if (enclosing_ != nullptr) return enclosing_->get(name);

    throw Interpreter::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, const Value& value) {
    if (values_.find(name.lexeme) != values_.end()) {
        if (!values_.at(name.lexeme).is_mutable) {
            throw Interpreter::RuntimeError(name, "Cannot assign to immutable variable '" + name.lexeme + "'.");
        }
        values_[name.lexeme].value = value;
        return;
    }

    if (enclosing_ != nullptr) {
        enclosing_->assign(name, value);
        return;
    }

    throw Interpreter::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::traceReferences(VM& vm) {
    for (auto const& [key, val] : values_) {
        vm.markValue(val.value);
    }
    if (enclosing_ != nullptr) {
        vm.markObject(enclosing_);
    }
}

size_t Environment::size() const {
    size_t totalSize = sizeof(Environment);
    for (const auto& pair : values_) {
        totalSize += pair.first.capacity();
        totalSize += sizeof(VariableInfo); // Add size of VariableInfo struct
    }
    return totalSize;
}

} // namespace nota
