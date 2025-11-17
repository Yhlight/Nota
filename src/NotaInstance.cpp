#include "NotaInstance.h"
#include "NotaClass.h"
#include "NotaFunction.h"
#include <stdexcept>

namespace nota {

NotaInstance::NotaInstance(NotaClass *klass) : klass(klass) {}

Value NotaInstance::get(const Token &name) {
    if (fields.count(name.lexeme)) {
        return fields[name.lexeme];
    }

    NotaFunction *method = klass->find_method(name.lexeme);
    if (method != nullptr) {
        return method->bind(this);
    }

    throw std::runtime_error("Undefined property '" + name.lexeme + "'.");
}

void NotaInstance::set(const Token &name, Value value) {
    fields[name.lexeme] = value;
}

} // namespace nota
