#include "VM.h"
#include <iostream>

// Helper for printing Values
std::ostream& operator<<(std::ostream& os, const Value& value) {
    std::visit([&os](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            os << "none";
        } else if constexpr (std::is_same_v<T, std::string>) {
            os << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            os << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            os << (arg ? "true" : "false");
        }
    }, value);
    return os;
}

VM::VM() : chunk_(nullptr), ip_(nullptr) {}

InterpretResult VM::interpret(const Chunk& chunk) {
    chunk_ = &chunk;
    ip_ = chunk_->get_code().data();
    return run();
}

void VM::push(Value value) {
    stack_.push_back(value);
}

Value VM::pop() {
    Value value = stack_.back();
    stack_.pop_back();
    return value;
}

Value VM::peek(int distance) {
    return stack_[stack_.size() - 1 - distance];
}

InterpretResult VM::run() {
#define READ_BYTE() (*ip_++)
#define READ_CONSTANT() (chunk_->get_constants()[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
        if (!std::holds_alternative<double>(peek(0)) || !std::holds_alternative<double>(peek(1))) { \
            std::cerr << "Runtime error: Operands must be numbers." << std::endl; \
            return InterpretResult::RUNTIME_ERROR; \
        } \
        double b = std::get<double>(pop()); \
        double a = std::get<double>(pop()); \
        push(a op b); \
    } while (false)

    for (;;) {
        uint8_t instruction = READ_BYTE();
        switch (static_cast<OpCode>(instruction)) {
            case OpCode::OP_RETURN: {
                return InterpretResult::OK;
            }
            case OpCode::OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OpCode::OP_NEGATE: {
                if (stack_.empty() || !std::holds_alternative<double>(peek())) {
                    std::cerr << "Runtime error: Operand must be a number." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                double value = std::get<double>(pop());
                push(-value);
                break;
            }
            case OpCode::OP_ADD:      BINARY_OP(+); break;
            case OpCode::OP_SUBTRACT: BINARY_OP(-); break;
            case OpCode::OP_MULTIPLY: BINARY_OP(*); break;
            case OpCode::OP_DIVIDE:   BINARY_OP(/); break;
            case OpCode::OP_DEFINE_GLOBAL: {
                std::string name = std::get<std::string>(READ_CONSTANT());
                globals_[name] = peek();
                pop();
                break;
            }
            case OpCode::OP_GET_GLOBAL: {
                std::string name = std::get<std::string>(READ_CONSTANT());
                if (globals_.find(name) == globals_.end()) {
                    std::cerr << "Runtime error: Undefined variable '" << name << "'." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                push(globals_[name]);
                break;
            }
            case OpCode::OP_PRINT: {
                std::cout << pop() << std::endl;
                break;
            }
            case OpCode::OP_POP: {
                pop();
                break;
            }
            default:
                std::cerr << "Unknown opcode: " << instruction << std::endl;
                return InterpretResult::RUNTIME_ERROR;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}
