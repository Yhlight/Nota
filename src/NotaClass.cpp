#include "NotaClass.h"
#include "NotaInstance.h"
#include "NotaFunction.h"

namespace nota {

NotaClass::NotaClass(std::string name, std::map<std::string, NotaFunction *> methods)
    : name(name), methods(methods) {}

Value NotaClass::call(Interpreter &interpreter, std::vector<Value> arguments) {
    auto instance = new NotaInstance(this);
    NotaFunction *initializer = find_method("new");
    if (initializer != nullptr) {
        initializer->bind(instance)->call(interpreter, arguments);
    }
    return instance;
}

int NotaClass::arity() {
    NotaFunction *initializer = find_method("new");
    if (initializer != nullptr) {
        return initializer->arity();
    }
    return 0;
}

NotaFunction *NotaClass::find_method(const std::string &name) {
    if (methods.count(name)) {
        return methods[name];
    }
    return nullptr;
}

} // namespace nota
