#pragma once

#include "NotaObjects.h"
#include "VM.h"
#include "Interpreter.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>

namespace nota::ffi {

// --- Unmarshal ---

template <typename T>
T Unmarshal(const Value& value);

template <>
inline int Unmarshal<int>(const Value& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    }
    throw std::runtime_error("Type mismatch: Expected int.");
}

template <>
inline double Unmarshal<double>(const Value& value) {
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    throw std::runtime_error("Type mismatch: Expected double.");
}

template <>
inline bool Unmarshal<bool>(const Value& value) {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    throw std::runtime_error("Type mismatch: Expected bool.");
}

template <>
inline std::string Unmarshal<std::string>(const Value& value) {
    if (auto obj = std::get_if<Object*>(&value)) {
        if (auto str = dynamic_cast<NotaString*>(*obj)) {
            return str->value;
        }
    }
    throw std::runtime_error("Type mismatch: Expected string.");
}

// --- Marshal ---

template <typename T>
Value Marshal(VM& vm, T value);

inline Value MarshalVoid(VM& vm) {
    return std::monostate{};
}

template <>
inline Value Marshal<int>(VM& vm, int value) {
    return value;
}

template <>
inline Value Marshal<double>(VM& vm, double value) {
    return value;
}

template <>
inline Value Marshal<bool>(VM& vm, bool value) {
    return value;
}

template <>
inline Value Marshal<std::string>(VM& vm, std::string value) {
    return vm.newObject<NotaString>(std::move(value));
}

template <>
inline Value Marshal<const char*>(VM& vm, const char* value) {
    return vm.newObject<NotaString>(value);
}

// --- NativeCaller ---

template<typename R, typename... Args>
class NativeCaller {
public:
    static Value Call(VM& vm, R (*func)(Args...), const std::vector<Value>& args_from_vm) {
        if (args_from_vm.size() != sizeof...(Args)) {
            throw std::runtime_error("Native function argument count mismatch.");
        }

        return [&]<size_t... Is>(std::index_sequence<Is...>) -> Value {
            R result = func(Unmarshal<Args>(args_from_vm[Is])...);
            return Marshal(vm, result);
        }(std::make_index_sequence<sizeof...(Args)>{});
    }
};

template<typename... Args>
class NativeCaller<void, Args...> {
public:
    static Value Call(VM& vm, void (*func)(Args...), const std::vector<Value>& args_from_vm) {
        if (args_from_vm.size() != sizeof...(Args)) {
            throw std::runtime_error("Native function argument count mismatch.");
        }

        [&]<size_t... Is>(std::index_sequence<Is...>) {
            func(Unmarshal<Args>(args_from_vm[Is])...);
        }(std::make_index_sequence<sizeof...(Args)>{});

        return MarshalVoid(vm);
    }
};

} // namespace nota::ffi
