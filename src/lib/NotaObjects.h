#pragma once

#include "Token.h"
#include "Object.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace nota {

// Forward declarations
class Interpreter;
struct FunctionStmt;
class NotaInstance;
class NotaClass;
class Callable;
class Environment;
class NotaFunction;
class NotaString;

using Value =
    std::variant<std::monostate, int, double, bool, Object*>;

class Callable : public Object {
public:
  virtual int arity() = 0;
  virtual Value call(Interpreter &interpreter,
                     std::vector<Value> arguments) = 0;
};

// Runtime representation of a string
class NotaString : public Object {
public:
    NotaString(std::string value) : value(value) {}
    std::string value;
    size_t size() const override;
};

// Runtime representation of a function
class NotaFunction : public Callable {
public:
  NotaFunction(FunctionStmt* declaration, Environment* closure, bool isInitializer = false);
  int arity() override;
  Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
  NotaFunction* bind(Interpreter& interpreter, NotaInstance* instance);
  std::string toString();
  bool isInitializer_;

  void traceReferences(VM& vm) override;
    size_t size() const override;

private:
  FunctionStmt* declaration_;
  Environment* closure_;
};


// Runtime representation of an instance
class NotaInstance : public Object {
public:
  NotaInstance(NotaClass* klass);
  ~NotaInstance();

  Value get(Interpreter& interpreter, const Token &name);
  void set(const Token &name, Value value);
  std::string toString();

  void traceReferences(VM& vm) override;
    size_t size() const override;


private:
  NotaClass* klass_;
  // PImpl idiom to break circular dependency with Value
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};

// Runtime representation of a class
class NotaClass : public Callable {
public:
  NotaClass(Token name, std::map<std::string, NotaFunction*> methods);

  int arity() override;
  Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
  std::string toString();
  NotaFunction* findMethod(const std::string& name);

  void traceReferences(VM& vm) override;
    size_t size() const override;

private:
  Token name_;
  std::map<std::string, NotaFunction*> methods_;
};

} // namespace nota
