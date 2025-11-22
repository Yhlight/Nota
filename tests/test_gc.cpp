#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"
#include "VM.h"
#include "vendor/doctest.h"
#include <string>

TEST_CASE("Garbage Collection") {
  std::string source = R"(
    class MyClass
      fn init(name)
        this.name = name;
      end
    end

    for (mut i = 0; i < 1000; i++)
      MyClass("instance");
    end
  )";

  auto lexer = std::make_shared<nota::Lexer>(source);
  auto tokens = lexer->scanTokens();
  auto parser = std::make_shared<nota::Parser>(tokens);
  auto statements = parser->parse();

  nota::VM vm;
  auto interpreter = std::make_shared<nota::Interpreter>(vm);

  // Before running, the VM should have very few objects (like the global env)
  CHECK(vm.getObjectCount() < 10);

  interpreter->interpret(statements);

  // After running the script but before a major collection, many objects should exist.
  // The exact number isn't critical, but it should be over 1000.
  CHECK(vm.getObjectCount() > 1000);

  // Manually trigger a garbage collection
  vm.collectGarbage();

  // After collection, only the class and the global environment should remain.
  // All the instances created in the loop should be gone.
  CHECK(vm.getObjectCount() < 10);
}

TEST_CASE("Circular Reference Garbage Collection") {
    std::string source = R"(
        let my_none;
        class A
            fn init()
                this.b = my_none;
            end
        end

        class B
            fn init()
                this.a = my_none;
            end
        end

        if true
            mut a = A();
            mut b = B();
            a.b = b;
            b.a = a;
        end
    )";

    nota::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    nota::Parser parser(tokens);
    auto statements = parser.parse();
    nota::VM vm;
    auto interpreter = std::make_shared<nota::Interpreter>(vm);

    interpreter->interpret(statements);

    vm.collectGarbage();
    // After collection, the circularly referenced objects should be gone.
    // The classes A and B, plus the global environment, should remain.
    CHECK(vm.getObjectCount() < 10);
}
