#ifndef NOTA_VM_HPP
#define NOTA_VM_HPP

#include "core/Chunk.hpp"
#include "core/NotaValue.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "core/NotaObject.hpp"

namespace nota {
namespace vm {

struct Global {
    core::NotaValue value;
    bool is_mutable;
};

struct NotaStringPtrHash {
    std::size_t operator()(const core::NotaString* s) const {
        return std::hash<std::string>()(s->GetChars());
    }
};

struct NotaStringPtrEqual {
    bool operator()(const core::NotaString* lhs, const core::NotaString* rhs) const {
        return lhs->GetChars() == rhs->GetChars();
    }
};


class VM {
public:
    enum InterpretResult {
        INTERPRET_OK,
        INTERPRET_COMPILE_ERROR,
        INTERPRET_RUNTIME_ERROR
    };

    ~VM();
    InterpretResult Interpret(const std::string& source);
    core::NotaValue Pop();

private:
    void Push(const core::NotaValue& value);
    core::NotaString* InternString(const std::string& str);

public:
    std::unordered_map<std::string, core::NotaString*> interned_strings_;
private:
    core::Chunk* chunk_ = nullptr;
    uint8_t* ip_ = nullptr;
    std::vector<core::NotaValue> stack_;
    std::unordered_map<core::NotaString*, Global, NotaStringPtrHash, NotaStringPtrEqual> globals_;
};

} // namespace vm
} // namespace nota

#endif // NOTA_VM_HPP
