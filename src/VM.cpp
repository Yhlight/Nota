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
        }
    }
}

} // namespace nota
