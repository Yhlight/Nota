#include "vm/VM.hpp"
#include "compiler/Compiler.hpp"

namespace nota {
namespace vm {

InterpretResult VM::Interpret(const std::string& source) {
    compiler::Compiler compiler;
    if (!compiler.Compile(source, chunk_)) {
        return InterpretResult::COMPILE_ERROR;
    }

    // TODO: Implement the VM execution logic here.

    return InterpretResult::OK;
}

} // namespace vm
} // namespace nota
