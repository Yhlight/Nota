#include "Value.h"

Value::Value() : value(nullptr) {}
Value::Value(double value) : value(value) {}
Value::Value(bool value) : value(value) {}
Value::Value(Object* value) : value(value) {}
Value::Value(std::nullptr_t) : value(nullptr) {}

bool Value::is_double() const { return std::holds_alternative<double>(value); }
bool Value::is_bool() const { return std::holds_alternative<bool>(value); }
bool Value::is_nil() const { return std::holds_alternative<std::nullptr_t>(value); }
bool Value::is_object() const { return std::holds_alternative<Object*>(value); }

double Value::as_double() const { return std::get<double>(value); }
bool Value::as_bool() const { return std::get<bool>(value); }
Object* Value::as_object() const { return std::get<Object*>(value); }