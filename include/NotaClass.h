#ifndef NOTA_CLASS_H
#define NOTA_CLASS_H

#include "Value.h"
#include "Interpreter.h"
#include <map>
#include <string>

namespace nota {

class NotaInstance;

class NotaClass : public Callable {
  public:
    NotaClass(std::string name, std::map<std::string, class NotaFunction *> methods);

    Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
    int arity() override;
    NotaFunction *find_method(const std::string &name);

  private:
    std::string name;
    std::map<std::string, class NotaFunction *> methods;
};

} // namespace nota

#endif // NOTA_CLASS_H
