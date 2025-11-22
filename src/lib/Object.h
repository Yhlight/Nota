#pragma once

namespace nota {

class VM;

class Object {
public:
    virtual ~Object() = default;

    bool isMarked = false;

    // For GC tracing
    virtual void traceReferences(VM& vm) {}
};

} // namespace nota
