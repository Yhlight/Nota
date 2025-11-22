#include "NotaObjects.h"
#include "AST.h"
#include "Environment.h"
#include "Interpreter.h"
#include "VM.h"
#include <utility>

namespace nota {

size_t NotaString::size() const {
    return sizeof(NotaString) + value.capacity();
}

void NotaArray::traceReferences(VM& vm) {
    for (const auto& element : elements) {
        vm.markValue(element);
    }
}

size_t NotaArray::size() const {
    return sizeof(NotaArray) + elements.capacity() * sizeof(Value);
}

// --- NotaFunction ---
NotaFunction::NotaFunction(std::shared_ptr<FunctionStmt> declaration,
                           Environment* closure,
                           bool isInitializer)
    : declaration_(declaration), closure_(closure),
      isInitializer_(isInitializer) {}

int NotaFunction::arity() { return declaration_->params.size(); }

Value NotaFunction::call(Interpreter &interpreter,
                         std::vector<Value> arguments) {
  auto environment = interpreter.vm.newObject<Environment>(closure_);
  for (int i = 0; i < declaration_->params.size(); ++i) {
    environment->define(declaration_->params[i].lexeme, arguments[i]);
  }

  try {
    interpreter.executeBlock(declaration_->body, environment);
  } catch (Interpreter::ReturnControl &returnValue) {
    if (isInitializer_) return closure_->get({TokenType::THIS, "this", {}, -1});
    return returnValue.value;
  }

  if (isInitializer_) return closure_->get({TokenType::THIS, "this", {}, -1});

  return {};
}

NotaFunction*
NotaFunction::bind(Interpreter& interpreter, NotaInstance* instance) {
  auto environment = interpreter.vm.newObject<Environment>(closure_);
  environment->define("this", instance);
  return interpreter.vm.newObject<NotaFunction>(declaration_, environment, isInitializer_);
}

std::string NotaFunction::toString() {
    return "<fn " + declaration_->name.lexeme + ">";
}

void NotaFunction::traceReferences(VM& vm) {
    // We don't trace the declaration, because it's part of the AST and has a different lifetime
    // We do trace the closure, because it's a heap-allocated environment
    if (closure_ != nullptr) {
        vm.markObject(closure_);
    }
}

size_t NotaFunction::size() const {
    return sizeof(NotaFunction);
}

// --- NotaInstance PImpl ---
struct NotaInstance::Impl {
  std::map<std::string, Value> fields;
};

// --- NotaInstance ---
NotaInstance::NotaInstance(NotaClass* klass)
    : klass_(klass), pimpl_(std::make_unique<Impl>()) {}

NotaInstance::~NotaInstance() = default;

Value NotaInstance::get(Interpreter& interpreter, const Token &name) {
  auto it = pimpl_->fields.find(name.lexeme);
  if (it != pimpl_->fields.end()) {
    return it->second;
  }

  auto method = klass_->findMethod(name.lexeme);
  if (method) return method->bind(interpreter, this);

  throw Interpreter::RuntimeError(name, "Undefined property '" + name.lexeme +
                                            "'.");
}

void NotaInstance::set(const Token &name, Value value) {
  pimpl_->fields[name.lexeme] = std::move(value);
}

std::string NotaInstance::toString() {
    return klass_->toString() + " instance";
}

void NotaInstance::traceReferences(VM& vm) {
    vm.markObject(klass_);
    for (auto const& [key, val] : pimpl_->fields) {
        vm.markValue(val);
    }
}

size_t NotaInstance::size() const {
    size_t totalSize = sizeof(NotaInstance);
    for (const auto& pair : pimpl_->fields) {
        totalSize += pair.first.capacity();
    }
    return totalSize;
}

// --- NotaClass ---
NotaClass::NotaClass(
    Token name, std::map<std::string, NotaFunction*> methods)
    : name_(std::move(name)), methods_(std::move(methods)) {}

int NotaClass::arity() {
  auto initializer = findMethod("init");
  if (initializer) return initializer->arity();
  return 0;
}

Value NotaClass::call(Interpreter &interpreter, std::vector<Value> arguments) {
  auto instance = interpreter.vm.newObject<NotaInstance>(this);
  auto initializer = findMethod("init");
  if (initializer) {
      initializer->bind(interpreter, instance)->call(interpreter, arguments);
  }
  return instance;
}

NotaFunction* NotaClass::findMethod(const std::string& name) {
    auto it = methods_.find(name);
    if (it != methods_.end()) {
        return it->second;
    }
    return nullptr;
}


std::string NotaClass::toString() { return name_.lexeme; }

void NotaClass::traceReferences(VM& vm) {
    for (auto const& [key, val] : methods_) {
        vm.markObject(val);
    }
}

size_t NotaClass::size() const {
    size_t totalSize = sizeof(NotaClass);
    for (const auto& pair : methods_) {
        totalSize += pair.first.capacity();
    }
    return totalSize;
}

// --- NotaNativeFunction ---
NotaNativeFunction::NotaNativeFunction(int arity, NativeFn function)
    : arity_(arity), function_(std::move(function)) {}

int NotaNativeFunction::arity() {
    return arity_;
}

Value NotaNativeFunction::call(Interpreter& interpreter, std::vector<Value> arguments) {
    return function_(interpreter, std::move(arguments));
}

size_t NotaNativeFunction::size() const {
    // This is an approximation, as the size of the std::function is not easily known.
    return sizeof(NotaNativeFunction);
}

} // namespace nota
