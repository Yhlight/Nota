#pragma once

#include "Object.h"
#include "NotaObjects.h"
#include <vector>

namespace nota {

class Interpreter;

class VM {
public:
    ~VM();

    template<typename T, typename... Args>
    T* newObject(Args&&... args);

    void collectGarbage(Interpreter& interpreter);

    void markValue(Value value);
    void markObject(Object* object);

private:
    void sweep();

    std::vector<Object*> objects_;

public:
    size_t getObjectCount() const { return objects_.size(); }
};

template<typename T, typename... Args>
T* VM::newObject(Args&&... args) {
    T* object = new T(std::forward<Args>(args)...);
    objects_.push_back(object);
    return object;
}

} // namespace nota
