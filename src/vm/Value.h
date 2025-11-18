#pragma once

#include "Object.h"
#include <variant>

class Value {
public:
    Value();
    Value(double value);
    Value(bool value);
    Value(Object* value);
    Value(std::nullptr_t);

    bool is_double() const;
    bool is_bool() const;
    bool is_nil() const;
    bool is_object() const;

    double as_double() const;
    bool as_bool() const;
    Object* as_object() const;

private:
    std::variant<double, bool, std::nullptr_t, Object*> value;
};
