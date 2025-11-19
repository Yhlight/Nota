#include "vm/VM.hpp"
#include "compiler/Compiler.hpp"
#include "parser/Parser.hpp"
#include "lexer/Lexer.hpp"
#include "core/NotaObject.hpp"

#define BINARY_OP(op) \
    do { \
        core::NotaValue b = Pop(); \
        core::NotaValue a = Pop(); \
        Push(core::NotaValue(a.AsInt() op b.AsInt())); \
    } while (false)

namespace nota {
namespace vm {

VM::~VM() {
    for (auto& pair : interned_strings_) {
        delete pair.second;
    }
}

VM::InterpretResult VM::Interpret(const std::string& source) {
    lexer::Lexer l(source);
    parser::Parser p(l);
    auto program = p.ParseProgram();
    if (!p.Errors().empty()) {
        return INTERPRET_COMPILE_ERROR;
    }

    core::Chunk chunk;
    compiler::Compiler compiler;
    if (!compiler.Compile(*program, chunk)) {
        return INTERPRET_COMPILE_ERROR;
    }

    chunk_ = &chunk;
    ip_ = &chunk_->code[0];

    for (;;) {
        uint8_t instruction = *ip_++;
        switch (instruction) {
            case core::OP_CONSTANT: {
                core::NotaValue constant = chunk_->constants[*ip_++];
                Push(constant);
                break;
            }
            case core::OP_ADD:      BINARY_OP(+); break;
            case core::OP_SUBTRACT: BINARY_OP(-); break;
            case core::OP_MULTIPLY: BINARY_OP(*); break;
            case core::OP_DIVIDE:   BINARY_OP(/); break;
            case core::OP_DEFINE_IMMUTABLE_GLOBAL: {
                core::NotaValue value = Pop();
                core::NotaString* name = InternString(chunk_->constants[*ip_++].AsString());
                globals_[name] = {value, false};
                break;
            }
            case core::OP_DEFINE_MUTABLE_GLOBAL: {
                core::NotaValue value = Pop();
                core::NotaString* name = InternString(chunk_->constants[*ip_++].AsString());
                globals_[name] = {value, true};
                break;
            }
            case core::OP_GET_GLOBAL: {
                core::NotaString* name = InternString(chunk_->constants[*ip_++].AsString());
                auto it = globals_.find(name);
                if (it == globals_.end()) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                Push(it->second.value);
                break;
            }
            case core::OP_SET_GLOBAL: {
                core::NotaValue value = Pop();
                core::NotaString* name = InternString(chunk_->constants[*ip_++].AsString());
                auto it = globals_.find(name);
                if (it == globals_.end()) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                if (!it->second.is_mutable) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                it->second.value = value;
                Push(value); // Push the value back on the stack for chained assignments
                break;
            }
            case core::OP_RETURN: {
                return INTERPRET_OK;
            }
        }
    }
}

core::NotaValue VM::Pop() {
    core::NotaValue value = stack_.back();
    stack_.pop_back();
    return value;
}

void VM::Push(const core::NotaValue& value) {
    stack_.push_back(value);
}

core::NotaString* VM::InternString(const std::string& str) {
    auto it = interned_strings_.find(str);
    if (it != interned_strings_.end()) {
        return it->second;
    }
    auto new_str = new core::NotaString(str);
    interned_strings_[str] = new_str;
    return new_str;
}

} // namespace vm
} // namespace nota
