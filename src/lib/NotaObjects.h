/**
 * @file NotaObjects.h
 * @brief Defines the runtime representations of Nota objects.
 */

#pragma once

#include "Token.h"
#include "Object.h"
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <functional>

namespace nota {

// Forward declarations
class Interpreter;
struct FunctionStmt;
class NotaInstance;
class NotaClass;
class Callable;
class Environment;
class NotaFunction;
class NotaNativeFunction;
class NotaString;

/**
 * @brief A variant that can hold any Nota value.
 */
using Value =
    std::variant<std::monostate, int, double, bool, Object*>;

/**
 * @class Callable
 * @brief An abstract base class for all callable objects in Nota.
 */
class Callable : public Object {
public:
    Callable(ObjectType type) : Object(type) {}
  /**
   * @brief Get the number of arguments the callable expects.
   * @return The arity of the callable.
   */
  virtual int arity() = 0;

  /**
   * @brief Call the object.
   * @param interpreter The interpreter instance.
   * @param arguments The arguments to the call.
   * @return The return value of the call.
   */
  virtual Value call(Interpreter &interpreter,
                     std::vector<Value> arguments) = 0;
};

/**
 * @class NotaString
 * @brief The runtime representation of a string.
 */
class NotaString : public Object {
public:
    NotaString(std::string value) : Object(ObjectType::STRING), value(value) {}
    std::string value;
    size_t size() const override;
};

/**
 * @class NotaFunction
 * @brief The runtime representation of a function.
 */
class NotaFunction : public Callable {
public:
  NotaFunction(std::shared_ptr<FunctionStmt> declaration, Environment* closure, bool isInitializer = false)
    : Callable(ObjectType::FUNCTION), declaration_(declaration), closure_(closure), isInitializer_(isInitializer) {}
  int arity() override;
  Value call(Interpreter &interpreter, std::vector<Value> arguments) override;
  NotaFunction* bind(Interpreter& interpreter, NotaInstance* instance);
  std::string toString();
  bool isInitializer_;

  void traceReferences(VM& vm) override;
    size_t size() const override;

private:
  std::shared_ptr<FunctionStmt> declaration_;
  Environment* closure_;
};


/**
 * @class NotaInstance
 * @brief The runtime representation of an instance of a class.
 */
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

/**
 * @class NotaClass
 * @brief The runtime representation of a class.
 */
class NotaClass : public Callable {
public:
  NotaClass(Token name, std::map<std::string, NotaFunction*> methods)
    : Callable(ObjectType::CLASS), name_(name), methods_(methods) {}

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

/**
 * @class NotaNativeFunction
 * @brief The runtime representation of a native C++ function.
 */
class NotaNativeFunction : public Callable {
public:
    using NativeFn = std::function<Value(Interpreter&, std::vector<Value>)>;

    NotaNativeFunction(int arity, NativeFn function)
        : Callable(ObjectType::NATIVE_FUNCTION), arity_(arity), function_(function) {}

    int arity() override;
    Value call(Interpreter& interpreter, std::vector<Value> arguments) override;
    size_t size() const override;

private:
    int arity_;
    NativeFn function_;
};

} // namespace nota
