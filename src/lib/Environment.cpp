#include "Environment.h"
#include "Interpreter.h"
#include <stdexcept>
#include "AST.h"

namespace nota {

void Environment::define(const std::string& name, const Value& value) {
    values_[name] = value;
}

Value Environment::get(const Token& name) {
    if (values_.find(name.lexeme) != values_.end()) {
        return values_.at(name.lexeme);
    }

    if (enclosing_ != nullptr) return enclosing_->get(name);

    throw Interpreter::RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, const Value& value) {
    if (values_.find(name.lexeme) != values_.end()) {
        values_[name.lexeme] = value;
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
        vm.markValue(val);
    }
    if (enclosing_ != nullptr) {
        vm.markObject(enclosing_);
    }
}

size_t Environment::size() const {
    size_t totalSize = sizeof(Environment);
    for (const auto& pair : values_) {
        totalSize += pair.first.capacity();
        // The size of the Value variant itself is already accounted for
        // by the map's size, but if it contains heap-allocated data,
        // that would need to be added.
    }
    return totalSize;
}

} // namespace nota
