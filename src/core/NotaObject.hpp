#pragma once

#include <cstdint>

namespace nota {
namespace core {

enum class ObjectType {
    STRING,
    // ... other object types will go here
};

class NotaObject {
public:
    explicit NotaObject(ObjectType type) : type_(type) {}
    virtual ~NotaObject() = default;

    ObjectType GetType() const { return type_; }

private:
    ObjectType type_;
    // GC data would go here in the future
};

} // namespace core
} // namespace nota
