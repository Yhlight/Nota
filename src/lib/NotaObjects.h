#pragma once

#include "Token.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace nota {

// Forward declarations to break cycles
class Interpreter;
struct FunctionStmt;
class NotaInstance;
class NotaClass;
class Callable;
class Environment;
class NotaFunction;

using Value =
    std::variant<std::monostate, int, double, std::string, bool,
                 std::shared_ptr<Callable>, std::shared_ptr<NotaInstance>>;

class Callable {
public:
  virtual ~Callable() = default;
  virtual int arity() = 0;
  virtual Value call(Interpreter &interpreter,
                     std::vector<Value> arguments) = 0;
};

// Runtime representation of a function
class NotaFunction : public Callable {
public:
  NotaFunction(std::shared_ptr<FunctionStmt> declaration, std::shared_ptr<Environment> closure, bool isInitializer = false);
  int arity() override;
  Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
  std::shared_ptr<NotaFunction> bind(std::shared_ptr<NotaInstance> instance);
  std::string toString();
  bool isInitializer_;

private:
  std::shared_ptr<FunctionStmt> declaration_;
  std::shared_ptr<Environment> closure_;
};


// Runtime representation of an instance
class NotaInstance : public std::enable_shared_from_this<NotaInstance> {
public:
  NotaInstance(std::shared_ptr<NotaClass> klass);
  ~NotaInstance();

  Value get(const Token &name);
  void set(const Token &name, Value value);
  std::string toString();


private:
  std::shared_ptr<NotaClass> klass_;
  // PImpl idiom to break circular dependency with Value
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};

// Runtime representation of a class
class NotaClass : public Callable,
                  public std::enable_shared_from_this<NotaClass> {
public:
  NotaClass(Token name, std::map<std::string, std::shared_ptr<NotaFunction>> methods);

  int arity() override;
  Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
  std::string toString();
  std::shared_ptr<NotaFunction> findMethod(const std::string& name);

private:
  Token name_;
  std::map<std::string, std::shared_ptr<NotaFunction>> methods_;
};

} // namespace nota
