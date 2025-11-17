#ifndef NOTA_INSTANCE_H
#define NOTA_INSTANCE_H

#include "Value.h"
#include "Token.h"
#include <map>
#include <string>

namespace nota {

class NotaClass;

class NotaInstance {
  public:
    NotaInstance(NotaClass *klass);

    Value get(const Token &name);
    void set(const Token &name, Value value);

  private:
    NotaClass *klass;
    std::map<std::string, Value> fields;
};

} // namespace nota

#endif // NOTA_INSTANCE_H
