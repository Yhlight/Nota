/**
 * @file Object.h
 * @brief The base class for all heap-allocated objects in Nota.
 */

#pragma once

#include <cstddef>

namespace nota {

/**
 * @enum ObjectType
 * @brief An enum to represent the type of a heap-allocated object.
 */
enum class ObjectType {
    STRING,
    FUNCTION,
    NATIVE_FUNCTION,
    CLASS,
    INSTANCE,
    ENVIRONMENT
};

class VM;

/**
 * @class Object
 * @brief The base class for all heap-allocated objects in Nota.
 *
 * This class provides the basic interface for garbage collection and runtime
 * type identification.
 */
class Object {
public:
    virtual ~Object() = default;

    bool isMarked = false;
    ObjectType type;

    /**
     * @brief Trace all references to other objects.
     * @param vm The VM instance.
     */
    virtual void traceReferences(VM& vm) {}

    /**
     * @brief Get the size of the object in bytes.
     * @return The size of the object.
     */
    virtual size_t size() const = 0;

protected:
    /**
     * @brief Construct a new Object.
     * @param type The type of the object.
     */
    Object(ObjectType type) : type(type) {}
};

} // namespace nota
