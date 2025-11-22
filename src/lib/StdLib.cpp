#include "StdLib.h"
#include "Object.h"
#include "NotaObjects.h"
#include "Interpreter.h"
#include <iostream>
#include <chrono>

namespace nota {
namespace stdlib {

// Helper to convert a Value to its string representation
std::string toString(const Value& value) {
    if (std::holds_alternative<std::monostate>(value)) {
        return "none";
    }
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    }
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    }
    if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    }
    if (std::holds_alternative<Object*>(value)) {
        Object* obj = std::get<Object*>(value);
        switch (obj->type) {
            case ObjectType::STRING:
                return static_cast<NotaString*>(obj)->value;
            case ObjectType::FUNCTION:
                return static_cast<NotaFunction*>(obj)->toString();
            case ObjectType::NATIVE_FUNCTION:
                return "<native fn>";
            case ObjectType::CLASS:
                return static_cast<NotaClass*>(obj)->toString();
            case ObjectType::INSTANCE:
                return static_cast<NotaInstance*>(obj)->toString();
            default:
                return "object";
        }
    }
    return "object";
}

Value print(Interpreter& interpreter, const std::vector<Value>& args) {
    for (size_t i = 0; i < args.size(); ++i) {
        std::cout << toString(args[i]);
        if (i < args.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    return {};
}

static const auto start_time = std::chrono::high_resolution_clock::now();

Value clock(Interpreter& interpreter, const std::vector<Value>& args) {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = now - start_time;
    return diff.count();
}

} // namespace stdlib
} // namespace nota
