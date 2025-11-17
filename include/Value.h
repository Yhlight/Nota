#ifndef NOTA_VALUE_H
#define NOTA_VALUE_H

#include <string>
#include <variant>
#include <vector>

namespace nota {

class Interpreter;
class Callable; // Forward declaration

using Value =
    std::variant<std::monostate, bool, long long, double, std::string,
                 Callable *>;

class Callable {
  public:
    virtual ~Callable() = default;
    virtual int arity() = 0;
    virtual Value call(Interpreter &interpreter,
                       std::vector<Value> arguments) = 0;
};

} // namespace nota

#endif // NOTA_VALUE_H
