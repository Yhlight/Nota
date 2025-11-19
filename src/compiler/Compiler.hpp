#ifndef NOTA_COMPILER_HPP
#define NOTA_COMPILER_HPP

#include "core/AST.hpp"
#include "core/Chunk.hpp"

namespace nota {
namespace compiler {

class Compiler {
public:
    bool Compile(const core::Program& program, core::Chunk& chunk);

private:
    void CompileNode(const core::Node* node);
    void CompileIntegerLiteral(const core::IntegerLiteral* literal);

    core::Chunk* compiling_chunk_ = nullptr;
};

} // namespace compiler
} // namespace nota

#endif // NOTA_COMPILER_HPP
