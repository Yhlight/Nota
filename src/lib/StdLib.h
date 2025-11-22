#pragma once

#include "NotaObjects.h"
#include "Interpreter.h"
#include <vector>

namespace nota {
namespace stdlib {

Value print(Interpreter& interpreter, const std::vector<Value>& args);
Value clock(Interpreter& interpreter, const std::vector<Value>& args);

} // namespace stdlib
} // namespace nota
