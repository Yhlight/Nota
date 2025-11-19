#pragma once

#include "core/AST.hpp"
#include "core/Chunk.hpp"
#include "parser/Parser.hpp"
#include <string>

namespace nota {
namespace compiler {

class Compiler {
public:
    bool Compile(const std::string& source, core::Chunk& chunk);

private:
    void CompileProgram(const core::Program* program);
    void CompileStatement(const core::Statement* stmt);
    void CompileExpression(const core::Expression* expr);

    void EmitByte(uint8_t byte);
    void EmitReturn();
    void EmitConstant(core::NotaValue value);

    uint8_t ParseVariable(const std::string& name);
    uint8_t IdentifierConstant(const std::string& name);

    core::Chunk* compiling_chunk_ = nullptr;
    std::unordered_map<std::string, uint8_t> globals_;
};

} // namespace compiler
} // namespace nota
