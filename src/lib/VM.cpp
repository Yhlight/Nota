#include "VM.h"
#include <iostream>
#include <cstdint>

namespace nota {

namespace { // Anonymous namespace for internal linkage

bool isTruthy(const Value& value) {
    if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value);
    }
    if (value.type() == typeid(nullptr)) {
        return false;
    }
    if (value.type() == typeid(double)) {
        return std::any_cast<double>(value) != 0.0;
    }
    return true; // Other types are truthy
}

bool isEqual(const Value& a, const Value& b) {
    if (a.type() != b.type()) return false;

    if (a.type() == typeid(nullptr)) return true; // nil == nil
    if (a.type() == typeid(bool)) {
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    }
    if (a.type() == typeid(double)) {
        return std::any_cast<double>(a) == std::any_cast<double>(b);
    }
    if (a.type() == typeid(std::string)) {
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    }

    return false;
}

} // namespace

VM::VM() {
    resetStack();
}

void VM::runtimeError(const std::string& message) {
    std::cerr << "Runtime error: " << message << std::endl;
}

void VM::resetStack() {
    stack.clear();
    ip = 0;
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    Value value = stack.back();
    stack.pop_back();
    return value;
}

InterpretResult VM::interpret(Chunk* chunk) {
    this->chunk = chunk;
    this->ip = 0;

    for (;;) {
        if (ip >= chunk->code.size()) {
            return InterpretResult::INTERPRET_OK;
        }
        uint8_t instruction = chunk->code[ip++];
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::OP_RETURN: {
                printValue(pop());
                std::cout << std::endl;
                return InterpretResult::INTERPRET_OK;
            }
            case OpCode::OP_CONSTANT: {
                Value constant = chunk->constants[chunk->code[ip++]];
                push(constant);
                break;
            }
            case OpCode::OP_NEGATE: {
                if (stack.back().type() != typeid(double)) {
                    runtimeError("Operand must be a number.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double value = std::any_cast<double>(pop());
                push(-value);
                break;
            }
            case OpCode::OP_ADD: {
                if (stack.back().type() != typeid(double) || stack[stack.size() - 2].type() != typeid(double)) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double b = std::any_cast<double>(pop());
                double a = std::any_cast<double>(pop());
                push(a + b);
                break;
            }
            case OpCode::OP_SUBTRACT: {
                if (stack.back().type() != typeid(double) || stack[stack.size() - 2].type() != typeid(double)) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double b = std::any_cast<double>(pop());
                double a = std::any_cast<double>(pop());
                push(a - b);
                break;
            }
            case OpCode::OP_MULTIPLY: {
                if (stack.back().type() != typeid(double) || stack[stack.size() - 2].type() != typeid(double)) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double b = std::any_cast<double>(pop());
                double a = std::any_cast<double>(pop());
                push(a * b);
                break;
            }
            case OpCode::OP_DIVIDE: {
                if (stack.back().type() != typeid(double) || stack[stack.size() - 2].type() != typeid(double)) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double b = std::any_cast<double>(pop());
                double a = std::any_cast<double>(pop());
                push(a / b);
                break;
            }
            case OpCode::OP_NOT:
                push(!isTruthy(pop()));
                break;
            case OpCode::OP_EQUAL: {
                Value b = pop();
                Value a = pop();
                push(isEqual(a, b));
                break;
            }
            case OpCode::OP_GREATER: {
                if (stack.back().type() != typeid(double) || stack[stack.size() - 2].type() != typeid(double)) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double b = std::any_cast<double>(pop());
                double a = std::any_cast<double>(pop());
                push(a > b);
                break;
            }
            case OpCode::OP_LESS: {
                if (stack.back().type() != typeid(double) || stack[stack.size() - 2].type() != typeid(double)) {
                    runtimeError("Operands must be numbers.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                double b = std::any_cast<double>(pop());
                double a = std::any_cast<double>(pop());
                push(a < b);
                break;
            }
            case OpCode::OP_DEFINE_GLOBAL: {
                std::string name = std::any_cast<std::string>(chunk->constants[chunk->code[ip++]]);
                globals[name] = pop();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                std::string name = std::any_cast<std::string>(chunk->constants[chunk->code[ip++]]);
                auto it = globals.find(name);
                if (it == globals.end()) {
                    runtimeError("Undefined variable '" + name + "'.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(it->second);
                break;
            }
            case OpCode::OP_SET_GLOBAL: {
                std::string name = std::any_cast<std::string>(chunk->constants[chunk->code[ip++]]);
                auto it = globals.find(name);
                if (it == globals.end()) {
                    runtimeError("Undefined variable '" + name + "'.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                it->second = stack.back();
                break;
            }
            case OpCode::OP_JUMP_IF_FALSE: {
                uint16_t offset = (chunk->code[ip] << 8) | chunk->code[ip + 1];
                ip += 2;
                if (!isTruthy(stack.back())) {
                    ip += offset;
                }
                break;
            }
            case OpCode::OP_JUMP: {
                uint16_t offset = (chunk->code[ip] << 8) | chunk->code[ip + 1];
                ip += 2;
                ip += offset;
                break;
            }
            case OpCode::OP_LOOP: {
                uint16_t offset = (chunk->code[ip] << 8) | chunk->code[ip + 1];
                ip += 2;
                ip -= offset;
                break;
            }
        }
    }
}

} // namespace nota
