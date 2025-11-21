#include "VM.h"
#include "Compiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "Object.h"
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
    callStack.clear();
}

void VM::push(Value value) {
    stack.push_back(value);
}

Value VM::pop() {
    lastPopped = stack.back();
    stack.pop_back();
    return lastPopped;
}

bool VM::call(std::shared_ptr<NotaFunction> function, int argCount) {
    if (argCount != function->arity) {
        runtimeError("Expected " + std::to_string(function->arity) +
                     " arguments but got " + std::to_string(argCount) + ".");
        return false;
    }

    CallFrame frame;
    frame.function = function;
    frame.ip = 0;
    frame.stackTop = stack.size() - argCount - 1;
    callStack.push_back(frame);
    return true;
}

InterpretResult VM::interpret(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto statements = parser.parse();

    if (statements.empty()) {
        return InterpretResult::INTERPRET_OK;
    }

    Compiler compiler;
    auto mainFunction = compiler.compile(statements);

    push(mainFunction);
    call(mainFunction, 0);

    return run();
}

InterpretResult VM::run() {
    for (;;) {
        CallFrame* frame = &callStack.back();
        Chunk* chunk = &frame->function->chunk;

        if (frame->ip >= chunk->code.size()) {
            return InterpretResult::INTERPRET_OK;
        }

        uint8_t instruction = chunk->code[frame->ip++];
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::OP_RETURN: {
                Value result = pop();
                if (callStack.size() == 1) {
                    // We're done.
                    return InterpretResult::INTERPRET_OK;
                }
                stack.resize(frame->stackTop);
                callStack.pop_back();
                push(result);
                break;
            }
            case OpCode::OP_CONSTANT: {
                Value constant = chunk->constants[chunk->code[frame->ip++]];
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
                std::string name = std::any_cast<std::string>(chunk->constants[chunk->code[frame->ip++]]);
                globals[name] = pop();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                std::string name = std::any_cast<std::string>(chunk->constants[chunk->code[frame->ip++]]);
                auto it = globals.find(name);
                if (it == globals.end()) {
                    runtimeError("Undefined variable '" + name + "'.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                push(it->second);
                break;
            }
            case OpCode::OP_SET_GLOBAL: {
                std::string name = std::any_cast<std::string>(chunk->constants[chunk->code[frame->ip++]]);
                auto it = globals.find(name);
                if (it == globals.end()) {
                    runtimeError("Undefined variable '" + name + "'.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                it->second = stack.back();
                break;
            }
            case OpCode::OP_GET_LOCAL: {
                uint8_t slot = chunk->code[frame->ip++];
                push(stack[frame->stackTop + slot]);
                break;
            }
            case OpCode::OP_SET_LOCAL: {
                uint8_t slot = chunk->code[frame->ip++];
                stack[frame->stackTop + slot] = stack.back();
                break;
            }
            case OpCode::OP_JUMP_IF_FALSE: {
                uint16_t offset = (chunk->code[frame->ip] << 8) | chunk->code[frame->ip + 1];
                frame->ip += 2;
                if (!isTruthy(pop())) {
                    frame->ip += offset;
                }
                break;
            }
            case OpCode::OP_JUMP: {
                uint16_t offset = (chunk->code[frame->ip] << 8) | chunk->code[frame->ip + 1];
                frame->ip += 2;
                frame->ip += offset;
                break;
            }
            case OpCode::OP_LOOP: {
                uint16_t offset = (chunk->code[frame->ip] << 8) | chunk->code[frame->ip + 1];
                frame->ip += 2;
                frame->ip -= offset;
                break;
            }
            case OpCode::OP_POP: {
                pop();
                break;
            }
            case OpCode::OP_CALL: {
                int argCount = chunk->code[frame->ip++];
                Value callee = stack[stack.size() - 1 - argCount];
                if (callee.type() != typeid(std::shared_ptr<NotaFunction>)) {
                    runtimeError("Can only call functions and classes.");
                    return InterpretResult::INTERPRET_RUNTIME_ERROR;
                }
                call(std::any_cast<std::shared_ptr<NotaFunction>>(callee), argCount);
                break;
            }
        }
    }
}

} // namespace nota
