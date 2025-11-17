#ifndef NOTA_VALUE_H
#define NOTA_VALUE_H

#include <string>
#include <variant>
#include <vector>
#include <memory>

namespace nota {

class Interpreter;
class Callable;
class NotaArray;
class NotaClass;
class NotaInstance;
class Environment;

using Value =
    std::variant<std::monostate, bool, long long, double, std::string,
                 Callable *, NotaArray *, NotaClass *, NotaInstance *, std::shared_ptr<Environment>>;

class Callable {
  public:
    virtual ~Callable() = default;
    virtual int arity() = 0;
    virtual Value call(Interpreter &interpreter,
                       std::vector<Value> arguments) = 0;
};

} // namespace nota

#endif // NOTA_VALUE_H
