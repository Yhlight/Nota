#include "VM.h"
#include "NotaObjects.h"
#include "Interpreter.h"
#include <iostream>

namespace nota {

VM::~VM() {
    for (auto object : objects_) {
        delete object;
    }
}

void VM::collectGarbage(Interpreter& interpreter) {
    interpreter.markRoots();
    sweep();
}

void VM::markValue(Value value) {
    if (std::holds_alternative<Object*>(value)) {
        markObject(std::get<Object*>(value));
    }
}

void VM::markObject(Object* object) {
    if (object == nullptr || object->isMarked) {
        return;
    }
    object->isMarked = true;
    object->traceReferences(*this);
}

void VM::sweep() {
    std::vector<Object*> survivors;
    for (auto object : objects_) {
        if (object->isMarked) {
            object->isMarked = false;
            survivors.push_back(object);
        } else {
            delete object;
        }
    }
    objects_ = survivors;
}

} // namespace nota
