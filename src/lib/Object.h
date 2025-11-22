#pragma once

#include <cstddef>

namespace nota {

enum class ObjectType {
    STRING,
    FUNCTION,
    NATIVE_FUNCTION,
    CLASS,
    INSTANCE,
    ENVIRONMENT
};

class VM;

class Object {
public:
    virtual ~Object() = default;

    bool isMarked = false;
    ObjectType type;

    // For GC tracing
    virtual void traceReferences(VM& vm) {}
    virtual size_t size() const = 0;

protected:
    Object(ObjectType type) : type(type) {}
};

} // namespace nota
