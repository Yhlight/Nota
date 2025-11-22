#pragma once

#include <cstddef>

namespace nota {

class VM;

class Object {
public:
    virtual ~Object() = default;

    bool isMarked = false;

    // For GC tracing
    virtual void traceReferences(VM& vm) {}
    virtual size_t size() const = 0;
};

} // namespace nota
