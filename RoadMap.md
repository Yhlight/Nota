# Nota Development Roadmap

## Phase 1: Core Language Features

*   [ ] **Task 1: Project Setup**
    *   [X] Initialize Git repository
    *   [ ] Set up CMake build system
    *   [ ] Create Python build script
    *   [ ] Configure `.gitignore`
*   [ ] **Task 2: Lexer (Tokenizer)**
    *   [ ] Implement lexer for basic tokens (keywords, identifiers, operators, literals)
    *   [ ] Add support for comments
    *   [ ] Write unit tests for the lexer
*   [ ] **Task 3: Parser**
    *   [ ] Implement LL(K) recursive descent parser
    *   [ ] Define Abstract Syntax Tree (AST) nodes
    *   [ ] Parse variable declarations (`let`, `mut`)
    *   [ ] Parse expressions and operators
    *   [ ] Parse control flow statements (`if`, `else`, `while`, `for`, `do-while`, `match`)
    *   [ ] Parse function definitions and calls
    *   [ ] Write unit tests for the parser
*   [ ] **Task 4: Bytecode VM**
    *   [ ] Design bytecode instruction set
    *   [ ] Implement a simple stack-based VM
    *   [ ] Implement instruction fetching and decoding
    *   [ ] Write unit tests for the VM
*   [ ] **Task 5: Compiler (AST to Bytecode)**
    *   [ ] Traverse AST to generate bytecode
    *   [ ] Implement symbol table for variable management
    *   [ ] Compile expressions and statements
    *   [ ] Write unit tests for the compiler
*   [ ] **Task 6: Garbage Collection**
    *   [ ] Implement a simple mark-and-sweep garbage collector
    *   [ ] Integrate GC with the VM
    *   [ ] Write unit tests for the GC

## Phase 2: Advanced Features

*   [ ] **Task 7: Data Types**
    *   [ ] Implement support for strings
    *   [ ] Implement support for arrays (static and dynamic)
*   [ ] **Task 8: Functions**
    *   [ ] Implement function objects
    *   [ ] Support for closures and lambda functions
*   [ ] **Task 9: Classes and Objects**
    *   [ ] Implement class definitions
    *   [ ] Implement object instantiation
    *   [ ] Support for methods and `this` keyword
*   [ ] **Task 10: Modules and Packages**
    *   [ ] Implement module system with `import`
    *   [ ] Implement package system
*   [ ] **Task 11: Foreign Function Interface (FFI)**
    *   [ ] Implement FFI for calling C++ functions from Nota
    *   [ ] Implement type marshaling

## Phase 3: Standard Library and Tooling

*   [ ] **Task 12: Standard Library**
    *   [ ] Implement basic I/O functions
    *   [ ] Implement string manipulation functions
    *   [ ] Implement math functions
*   [ ] **Task 13: Command-Line Interface (CLI)**
    *   [ ] Create a CLI for running Nota scripts
    *   [ ] Add support for file input and interactive mode
*   [ ] **Task 14: Debugger**
    *   [ ] Implement a simple debugger
    *   [ ] Support for breakpoints and stepping
