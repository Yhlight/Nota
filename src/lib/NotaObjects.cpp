#include "NotaObjects.h"
#include "AST.h"
#include "Environment.h"
#include "Interpreter.h"
#include <utility>

namespace nota {
// --- NotaFunction ---
NotaFunction::NotaFunction(std::shared_ptr<FunctionStmt> declaration,
                           std::shared_ptr<Environment> closure,
                           bool isInitializer)
    : declaration_(std::move(declaration)), closure_(std::move(closure)),
      isInitializer_(isInitializer) {}

int NotaFunction::arity() { return declaration_->params.size(); }

Value NotaFunction::call(Interpreter &interpreter,
                         std::vector<Value> arguments) {
  auto environment = std::make_shared<Environment>(closure_);
  for (int i = 0; i < declaration_->params.size(); ++i) {
    environment->define(declaration_->params[i].lexeme, arguments[i]);
  }

  try {
    interpreter.executeBlock(declaration_->body, environment);
  } catch (Interpreter::ReturnControl &returnValue) {
    if (isInitializer_) {
        // Fall through to the `this` return at the end of the function
    } else {
        return returnValue.value;
    }
  }

  if (isInitializer_) return closure_->get({TokenType::THIS, "this", {}, -1});

  return {};
}

std::shared_ptr<NotaFunction>
NotaFunction::bind(std::shared_ptr<NotaInstance> instance) {
  auto environment = std::make_shared<Environment>(closure_);
  environment->define("this", instance);
  return std::make_shared<NotaFunction>(declaration_, environment, isInitializer_);
}

std::string NotaFunction::toString() {
    return "<fn " + declaration_->name.lexeme + ">";
}

// --- NotaInstance PImpl ---
struct NotaInstance::Impl {
  std::map<std::string, Value> fields;
};

// --- NotaInstance ---
NotaInstance::NotaInstance(std::shared_ptr<NotaClass> klass)
    : klass_(std::move(klass)), pimpl_(std::make_unique<Impl>()) {}

NotaInstance::~NotaInstance() = default;

Value NotaInstance::get(const Token &name) {
  auto it = pimpl_->fields.find(name.lexeme);
  if (it != pimpl_->fields.end()) {
    return it->second;
  }

  auto method = klass_->findMethod(name.lexeme);
  if (method) return method->bind(shared_from_this());

  throw Interpreter::RuntimeError(name, "Undefined property '" + name.lexeme +
                                            "'.");
}

void NotaInstance::set(const Token &name, Value value) {
  pimpl_->fields[name.lexeme] = std::move(value);
}

std::string NotaInstance::toString() {
    return klass_->toString() + " instance";
}

// --- NotaClass ---
NotaClass::NotaClass(
    Token name, std::map<std::string, std::shared_ptr<NotaFunction>> methods)
    : name_(std::move(name)), methods_(std::move(methods)) {}

int NotaClass::arity() {
  auto initializer = findMethod("init");
  if (initializer) return initializer->arity();
  return 0;
}

Value NotaClass::call(Interpreter &interpreter, std::vector<Value> arguments) {
  auto instance = std::make_shared<NotaInstance>(shared_from_this());
  auto initializer = findMethod("init");
  if (initializer) {
      initializer->bind(instance)->call(interpreter, arguments);
  }
  return instance;
}

std::shared_ptr<NotaFunction> NotaClass::findMethod(const std::string& name) {
    auto it = methods_.find(name);
    if (it != methods_.end()) {
        return it->second;
    }
    return nullptr;
}


std::string NotaClass::toString() { return name_.lexeme; }

} // namespace nota
