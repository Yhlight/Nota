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

namespace {

void ComparisonOp(VM* vm, core::OpCode op) {
    core::NotaValue b = vm->Pop();
    core::NotaValue a = vm->Pop();
    bool result = false;

    if (a.IsInt() && b.IsInt()) {
        switch (op) {
            case core::OP_EQUAL:    result = a.AsInt() == b.AsInt(); break;
            case core::OP_NOT_EQUAL: result = a.AsInt() != b.AsInt(); break;
            case core::OP_GREATER:  result = a.AsInt() > b.AsInt(); break;
            case core::OP_GREATER_EQUAL: result = a.AsInt() >= b.AsInt(); break;
            case core::OP_LESS:     result = a.AsInt() < b.AsInt(); break;
            case core::OP_LESS_EQUAL: result = a.AsInt() <= b.AsInt(); break;
            default: break;
        }
    } else if (a.IsFloat() && b.IsFloat()) {
        switch (op) {
            case core::OP_EQUAL:    result = a.AsFloat() == b.AsFloat(); break;
            case core::OP_NOT_EQUAL: result = a.AsFloat() != b.AsFloat(); break;
            case core::OP_GREATER:  result = a.AsFloat() > b.AsFloat(); break;
            case core::OP_GREATER_EQUAL: result = a.AsFloat() >= b.AsFloat(); break;
            case core::OP_LESS:     result = a.AsFloat() < b.AsFloat(); break;
            case core::OP_LESS_EQUAL: result = a.AsFloat() <= b.AsFloat(); break;
            default: break;
        }
    } else if (a.IsInt() && b.IsFloat()) {
        switch (op) {
            case core::OP_EQUAL:    result = a.AsInt() == b.AsFloat(); break;
            case core::OP_NOT_EQUAL: result = a.AsInt() != b.AsFloat(); break;
            case core::OP_GREATER:  result = a.AsInt() > b.AsFloat(); break;
            case core::OP_GREATER_EQUAL: result = a.AsInt() >= b.AsFloat(); break;
            case core::OP_LESS:     result = a.AsInt() < b.AsFloat(); break;
            case core::OP_LESS_EQUAL: result = a.AsInt() <= b.AsFloat(); break;
            default: break;
        }
    } else if (a.IsFloat() && b.IsInt()) {
        switch (op) {
            case core::OP_EQUAL:    result = a.AsFloat() == b.AsInt(); break;
            case core::OP_NOT_EQUAL: result = a.AsFloat() != b.AsInt(); break;
            case core::OP_GREATER:  result = a.AsFloat() > b.AsInt(); break;
            case core::OP_GREATER_EQUAL: result = a.AsFloat() >= b.AsInt(); break;
            case core::OP_LESS:     result = a.AsFloat() < b.AsInt(); break;
            case core::OP_LESS_EQUAL: result = a.AsFloat() <= b.AsInt(); break;
            default: break;
        }
    } else if (a.IsBool() && b.IsBool()) {
        switch (op) {
            case core::OP_EQUAL:    result = a.AsBool() == b.AsBool(); break;
            case core::OP_NOT_EQUAL: result = a.AsBool() != b.AsBool(); break;
            default: break;
        }
    }

    vm->Push(core::NotaValue(result));
}

} // namespace

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
            case core::OP_EQUAL:    ComparisonOp(this, core::OP_EQUAL); break;
            case core::OP_NOT_EQUAL: ComparisonOp(this, core::OP_NOT_EQUAL); break;
            case core::OP_GREATER:  ComparisonOp(this, core::OP_GREATER); break;
            case core::OP_GREATER_EQUAL: ComparisonOp(this, core::OP_GREATER_EQUAL); break;
            case core::OP_LESS:     ComparisonOp(this, core::OP_LESS); break;
            case core::OP_LESS_EQUAL: ComparisonOp(this, core::OP_LESS_EQUAL); break;
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
