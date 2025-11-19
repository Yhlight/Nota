#pragma once

#include "NotaObject.hpp"
#include <variant>

namespace nota {
namespace core {

enum class ValueType {
    NIL,
    BOOL,
    INT,
    FLOAT,
    OBJECT,
};

class NotaValue {
public:
    NotaValue() : type_(ValueType::NIL), value_() {}
    explicit NotaValue(bool value) : type_(ValueType::BOOL), value_(value) {}
    explicit NotaValue(int value) : type_(ValueType::INT), value_(static_cast<int64_t>(value)) {}
    explicit NotaValue(int64_t value) : type_(ValueType::INT), value_(value) {}
    explicit NotaValue(double value) : type_(ValueType::FLOAT), value_(value) {}
    explicit NotaValue(NotaObject* value) : type_(ValueType::OBJECT), value_(value) {}

    ValueType GetType() const { return type_; }

    bool IsNil() const { return type_ == ValueType::NIL; }
    bool IsBool() const { return type_ == ValueType::BOOL; }
    bool IsInt() const { return type_ == ValueType::INT; }
    bool IsFloat() const { return type_ == ValueType::FLOAT; }
    bool IsObject() const { return type_ == ValueType::OBJECT; }

    bool AsBool() const { return std::get<bool>(value_); }
    int64_t AsInt() const { return std::get<int64_t>(value_); }
    double AsFloat() const { return std::get<double>(value_); }
    NotaObject* AsObject() const { return std::get<NotaObject*>(value_); }

private:
    ValueType type_;
    std::variant<std::monostate, bool, int64_t, double, NotaObject*> value_;
};

} // namespace core
} // namespace nota
