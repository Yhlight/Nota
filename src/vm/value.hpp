#pragma once

#include <variant>

enum class ValueType {
    BOOL,
    NUMBER,
};

using Value = std::variant<bool, double>;

void print_value(Value value);
