#include "VM.h"
#include <iostream>
#include "Value.h"
#include <iostream>

VM::VM() : chunk_(nullptr), ip_(nullptr) {}

VM::~VM() {
    Object* object = objects_;
    while (object != nullptr) {
        Object* next = object->next;
        delete object;
        object = next;
    }
}

template <typename T, typename... Args>
T* VM::allocate_object(Args&&... args) {
    T* object = new T(std::forward<Args>(args)...);
    object->next = objects_;
    objects_ = object;
    return object;
}

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

InterpretResult VM::run() {
#define READ_BYTE() (*ip_++)
#define READ_CONSTANT() (chunk_->get_constants()[READ_BYTE()])

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
                if (stack_.empty() || !stack_.back().is_double()) {
                    std::cerr << "Runtime error: Operand must be a number." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                push(-pop().as_double());
                break;
            }
            case OpCode::OP_ADD: {
                Value b = pop();
                Value a = pop();
                if (a.is_double() && b.is_double()) {
                    push(a.as_double() + b.as_double());
                } else if (a.is_object() && a.as_object()->type == ObjectType::STRING &&
                           b.is_object() && b.as_object()->type == ObjectType::STRING) {
                    auto s1 = static_cast<ObjectString*>(a.as_object());
                    auto s2 = static_cast<ObjectString*>(b.as_object());
                    push(allocate_object<ObjectString>(s1->chars + s2->chars));
                } else {
                    std::cerr << "Operands must be two numbers or two strings." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                break;
            }
            case OpCode::OP_SUBTRACT: {
                Value b = pop();
                Value a = pop();
                if (a.is_double() && b.is_double()) {
                    push(a.as_double() - b.as_double());
                } else {
                    std::cerr << "Operands must be numbers." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                break;
            }
            case OpCode::OP_MULTIPLY: {
                Value b = pop();
                Value a = pop();
                if (a.is_double() && b.is_double()) {
                    push(a.as_double() * b.as_double());
                } else {
                    std::cerr << "Operands must be numbers." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                break;
            }
            case OpCode::OP_DIVIDE: {
                Value b = pop();
                Value a = pop();
                if (a.is_double() && b.is_double()) {
                    push(a.as_double() / b.as_double());
                } else {
                    std::cerr << "Operands must be numbers." << std::endl;
                    return InterpretResult::RUNTIME_ERROR;
                }
                break;
            }
            default:
                std::cerr << "Unknown opcode: " << instruction << std::endl;
                return InterpretResult::RUNTIME_ERROR;
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
}
