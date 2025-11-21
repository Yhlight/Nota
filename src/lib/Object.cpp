#include "Object.h"

namespace nota {

NotaFunction::NotaFunction(std::string name, int arity)
    : name(std::move(name)), arity(arity) {}

} // namespace nota
