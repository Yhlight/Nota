#include "vendor/doctest.h"
#include "VM.h"
#include "NotaObjects.h"
#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"

TEST_CASE("Garbage Collection") {
    SUBCASE("Collects unreachable objects") {
        nota::VM vm;
        nota::Interpreter interpreter(vm);

        // The root environment is always reachable.
        CHECK(interpreter.vm.getObjectCount() == 1);

        interpreter.vm.newObject<nota::NotaString>("unreachable");
        CHECK(interpreter.vm.getObjectCount() == 2);

        interpreter.vm.collectGarbage(interpreter);

        // Only the root environment should remain.
        CHECK(interpreter.vm.getObjectCount() == 1);
    }

    SUBCASE("Collects circular references") {
        std::string source = R"(
            do
                class A
                    fn set_b(b)
                        this.b = b;
                    end
                end

                class B
                    fn set_a(a)
                        this.a = a;
                    end
                end

                let a = A();
                let b = B();
                a.set_b(b);
                b.set_a(a);
            while false;
        )";

        nota::Lexer lexer(source);
        auto tokens = lexer.scanTokens();
        nota::Parser parser(tokens);
        auto statements = parser.parse();
        nota::VM vm;
        auto interpreter = std::make_shared<nota::Interpreter>(vm);

        // The root environment is always reachable.
        CHECK(interpreter->vm.getObjectCount() == 1);

        interpreter->interpret(statements);

        interpreter->vm.collectGarbage(*interpreter);

        CHECK(interpreter->vm.getObjectCount() == 1);
    }
}
