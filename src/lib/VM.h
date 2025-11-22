#pragma once

#include "Object.h"
#include "NotaObjects.h"
#include <vector>

namespace nota {

class Interpreter;

class VM {
public:
    VM();
    ~VM();

    void setInterpreter(Interpreter* interpreter);

    template<typename T, typename... Args>
    T* newObject(Args&&... args);

    void collectGarbage();

    void markValue(Value value);
    void markObject(Object* object);

private:
    void sweep();

    std::vector<Object*> objects_;
    Interpreter* interpreter_ = nullptr;
    size_t bytesAllocated_ = 0;
    size_t nextGC_ = 1024 * 1024;
    static constexpr int kGCHeapGrowFactor = 2;


public:
    size_t getObjectCount() const { return objects_.size(); }
};

template<typename T, typename... Args>
T* VM::newObject(Args&&... args) {
    if (bytesAllocated_ > nextGC_) {
        collectGarbage();
    }
    T* object = new T(std::forward<Args>(args)...);
    objects_.push_back(object);
    bytesAllocated_ += object->size();
    return object;
}

} // namespace nota
