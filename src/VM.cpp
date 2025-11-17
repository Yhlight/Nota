#include "VM.h"
#include <iostream>

namespace nota {

InterpretResult VM::interpret(const Chunk &chunk) {
    this->chunk = chunk;
    this->ip = &this->chunk.code[0];

    for (;;) {
        uint8_t instruction = *ip++;
        switch ((OpCode)instruction) {
            case OpCode::Return: {
                return InterpretResult::Ok;
            }
            case OpCode::Constant: {
                Value constant = chunk.constants[*ip++];
                stack.push_back(constant);
                break;
            }
            case OpCode::True: {
                stack.push_back(true);
                break;
            }
            case OpCode::False: {
                stack.push_back(false);
                break;
            }
            case OpCode::Negate: {
                Value value = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg)>>) {
                        return -arg;
                    }
                    throw std::runtime_error("Operand must be a number.");
                }, value));
                break;
            }
            case OpCode::Not: {
                Value value = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg) -> Value {
                    if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, bool>) {
                        return !arg;
                    }
                    throw std::runtime_error("Operand must be a boolean.");
                }, value));
                break;
            }
            case OpCode::Add: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 + arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::Subtract: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 - arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::Multiply: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 * arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::Divide: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 / arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::Equal: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;
                    if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
                        return arg1 == arg2;
                    }
                    if constexpr (std::is_same_v<T1, std::string> && std::is_same_v<T2, std::string>) {
                        return arg1 == arg2;
                    }
                    if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) {
                        return arg1 == arg2;
                    }
                    return false;
                }, a, b));
                break;
            }
            case OpCode::NotEqual: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    using T1 = std::decay_t<decltype(arg1)>;
                    using T2 = std::decay_t<decltype(arg2)>;
                    if constexpr (std::is_arithmetic_v<T1> && std::is_arithmetic_v<T2>) {
                        return arg1 != arg2;
                    }
                    if constexpr (std::is_same_v<T1, std::string> && std::is_same_v<T2, std::string>) {
                        return arg1 != arg2;
                    }
                    if constexpr (std::is_same_v<T1, bool> && std::is_same_v<T2, bool>) {
                        return arg1 != arg2;
                    }
                    return true;
                }, a, b));
                break;
            }
            case OpCode::Greater: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 > arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::Less: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 < arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::GreaterEqual: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 >= arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
            case OpCode::LessEqual: {
                Value b = stack.back();
                stack.pop_back();
                Value a = stack.back();
                stack.pop_back();
                stack.push_back(std::visit([](auto&& arg1, auto&& arg2) -> Value {
                    if constexpr (std::is_arithmetic_v<std::decay_t<decltype(arg1)>> && std::is_arithmetic_v<std::decay_t<decltype(arg2)>>) {
                        return arg1 <= arg2;
                    }
                    throw std::runtime_error("Operands must be numbers.");
                }, a, b));
                break;
            }
        }
    }
}

Value VM::pop() {
    Value value = stack.back();
    stack.pop_back();
    return value;
}

} // namespace nota
