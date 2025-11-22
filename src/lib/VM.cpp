#include "VM.h"
#include "NotaObjects.h"
#include "Interpreter.h"
#include <iostream>

namespace nota {

VM::VM() = default;

VM::~VM() {
    for (auto object : objects_) {
        delete object;
    }
}

void VM::setInterpreter(Interpreter* interpreter) {
    interpreter_ = interpreter;
}

void VM::raiseError(const std::string& message) {
    throw Interpreter::RuntimeError(Token{TokenType::END_OF_FILE, "", {}, -1}, message);
}

void VM::collectGarbage() {
    if (interpreter_) {
        interpreter_->markRoots();
    }
    sweep();
    nextGC_ = bytesAllocated_ * kGCHeapGrowFactor;
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
    bytesAllocated_ = 0;
    for (auto object : objects_) {
        if (object->isMarked) {
            object->isMarked = false;
            survivors.push_back(object);
            bytesAllocated_ += object->size();
        } else {
            delete object;
        }
    }
    objects_ = survivors;
}

} // namespace nota
