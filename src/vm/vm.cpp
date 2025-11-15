#include "vm.hpp"
#include "../lexer.hpp"
#include "../parser.hpp"
#include "compiler.hpp"
#include <iostream>

namespace nota {

    VM::VM() : ip(nullptr) {}

    InterpretResult VM::interpret(const std::string& source) {
        Lexer lexer(source);
        std::vector<Token> tokens;
        for (Token token = lexer.scanToken(); token.type != TokenType::END_OF_FILE; token = lexer.scanToken()) {
            tokens.push_back(token);
        }
        tokens.push_back(lexer.scanToken());

        Parser parser(tokens);
        auto statements = parser.parse();

        Compiler compiler;
        chunk = compiler.compile(statements);
        if (!chunk) {
            return InterpretResult::COMPILE_ERROR;
        }

        ip = &chunk->getCode()[0];

        return run();
    }

    void VM::push(Value value) {
        stack.push_back(value);
    }

    Value VM::pop() {
        Value value = stack.back();
        stack.pop_back();
        return value;
    }

    Value VM::peek(int distance) {
        return stack[stack.size() - 1 - distance];
    }

    bool isFalsey(Value value) {
        if (std::holds_alternative<std::monostate>(value)) return true;
        if (std::holds_alternative<bool>(value)) return !std::get<bool>(value);
        return false;
    }

    template<typename T, typename U>
    bool valuesEqual(T a, U b) {
        if constexpr (std::is_same_v<T, U>) {
            return a == b;
        }
        return false;
    }

    InterpretResult VM::run() {
        #define READ_BYTE() (*ip++)
        #define READ_CONSTANT() (chunk->getConstants()[READ_BYTE()])
        #define READ_STRING() (std::get<std::string>(READ_CONSTANT()))
        #define BINARY_OP(valueType, op) \
            do { \
                if (!std::holds_alternative<double>(peek(0)) || !std::holds_alternative<double>(peek(1))) { \
                    std::cerr << "Operands must be numbers." << std::endl; \
                    return InterpretResult::RUNTIME_ERROR; \
                } \
                double b = std::get<double>(pop()); \
                double a = std::get<double>(pop()); \
                push(valueType(a op b)); \
            } while (false)

        for (;;) {
            uint8_t instruction = READ_BYTE();
            switch (static_cast<OpCode>(instruction)) {
                case OpCode::OP_RETURN: {
                    return InterpretResult::OK;
                }
                case OpCode::OP_POP: pop(); break;
                case OpCode::OP_CONSTANT: {
                    Value constant = READ_CONSTANT();
                    push(constant);
                    break;
                }
                case OpCode::OP_DEFINE_GLOBAL: {
                    std::string name = READ_STRING();
                    globals[name] = pop();
                    break;
                }
                case OpCode::OP_GET_GLOBAL: {
                    std::string name = READ_STRING();
                    auto it = globals.find(name);
                    if (it == globals.end()) {
                        std::cerr << "Undefined variable '" << name << "'." << std::endl;
                        return InterpretResult::RUNTIME_ERROR;
                    }
                    push(it->second);
                    break;
                }
                case OpCode::OP_SET_GLOBAL: {
                    std::string name = READ_STRING();
                    auto it = globals.find(name);
                    if (it == globals.end()) {
                        std::cerr << "Undefined variable '" << name << "'." << std::endl;
                        return InterpretResult::RUNTIME_ERROR;
                    }
                    it->second = peek(0);
                    break;
                }
                case OpCode::OP_NEGATE: {
                    if (!std::holds_alternative<double>(peek(0))) {
                        std::cerr << "Operand must be a number." << std::endl;
                        return InterpretResult::RUNTIME_ERROR;
                    }
                    push(-std::get<double>(pop()));
                    break;
                }
                case OpCode::OP_ADD:      BINARY_OP(double, +); break;
                case OpCode::OP_SUBTRACT: BINARY_OP(double, -); break;
                case OpCode::OP_MULTIPLY: BINARY_OP(double, *); break;
                case OpCode::OP_DIVIDE:   BINARY_OP(double, /); break;
                case OpCode::OP_NIL:      push(std::monostate{}); break;
                case OpCode::OP_TRUE:     push(true); break;
                case OpCode::OP_FALSE:    push(false); break;
                case OpCode::OP_NOT:      push(isFalsey(pop())); break;
                case OpCode::OP_EQUAL: {
                    Value b = pop();
                    Value a = pop();
                    push(std::visit([](auto&& arg1, auto&& arg2) { return valuesEqual(arg1, arg2); }, a, b));
                    break;
                }
                case OpCode::OP_GREATER:  BINARY_OP(bool, >); break;
                case OpCode::OP_LESS:     BINARY_OP(bool, <); break;
            }
        }
        #undef READ_BYTE
        #undef READ_CONSTANT
        #undef READ_STRING
        #undef BINARY_OP
    }

} // namespace nota
