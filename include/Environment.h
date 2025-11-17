#ifndef NOTA_ENVIRONMENT_H
#define NOTA_ENVIRONMENT_H

#include "Token.h"
#include "Value.h"
#include <map>
#include <memory>
#include <string>

namespace nota {

class Environment : public std::enable_shared_from_this<Environment> {
  public:
    Environment(std::shared_ptr<Environment> enclosing = nullptr);

    void define(const std::string &name, Value value);
    Value get(const Token &name);
    void assign(const Token &name, Value value);

  private:
    std::shared_ptr<Environment> enclosing;
    std::map<std::string, Value> values;
};

} // namespace nota

#endif // NOTA_ENVIRONMENT_H
