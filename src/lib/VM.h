/**
 * @file VM.h
 * @brief The virtual machine for the Nota language.
 */

#pragma once

#include "Object.h"
#include "NotaObjects.h"
#include <vector>
#include <cstddef>

namespace nota {

class Interpreter; // Forward declaration

/**
 * @class VM
 * @brief The virtual machine for the Nota language.
 *
 * The VM is responsible for memory management and object allocation.
 */
class VM {
public:
    VM();
    ~VM();

    /**
     * @brief Allocate a new object of type T.
     * @tparam T The type of the object to allocate.
     * @tparam Args The types of the arguments to the constructor.
     * @param args The arguments to the constructor.
     * @return A pointer to the new object.
     */
    template<typename T, typename... Args>
    T* newObject(Args&&... args) {
        if (bytesAllocated_ > nextGC_) {
            collectGarbage();
        }
        T* object = new T(std::forward<Args>(args)...);
        bytesAllocated_ += object->size();
        objects_.push_back(object);
        return object;
    }

    /**
     * @brief Set the interpreter instance.
     * @param interpreter The interpreter instance.
     */
    void setInterpreter(Interpreter* interpreter);

    /**
     * @brief Mark an object as reachable.
     * @param obj The object to mark.
     */
    void markObject(Object* obj);

    /**
     * @brief Mark a value as reachable.
     * @param value The value to mark.
     */
    void markValue(const Value& value);

    /**
     * @brief Raise a runtime error.
     * @param message The error message.
     */
    void raiseError(const std::string& message);

    /**
     * @brief Get the number of objects currently allocated.
     * @return The number of objects.
     */
    size_t getObjectCount() const { return objects_.size(); }

    /**
     * @brief Run the garbage collector.
     */
    void collectGarbage();

private:
    void traceReferences();
    void sweep();

    std::vector<Object*> objects_;
    Interpreter* interpreter_ = nullptr;
    size_t nextGC_ = 1024 * 1024;
    size_t bytesAllocated_ = 0;
    static constexpr double kGCHeapGrowFactor = 2.0;
};

} // namespace nota
