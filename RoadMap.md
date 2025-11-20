# Nota Development Roadmap

## Phase 1: Core Language Features

*   [X] **Task 1: Project Setup**
    *   [X] Initialize Git repository
    *   [X] Set up CMake build system
    *   [X] Create Python build script
    *   [X] Configure `.gitignore`
*   [X] **Task 2: Lexer (Tokenizer)**
    *   [X] Implement lexer for basic tokens (keywords, identifiers, operators, literals)
    *   [X] Add support for comments
    *   [X] Write unit tests for the lexer
*   [ ] **Task 3: Parser**
    *   [X] Implement LL(K) recursive descent parser
    *   [X] Define Abstract Syntax Tree (AST) nodes
    *   [ ] Parse variable declarations (`let`, `mut`)
    *   [X] Parse expressions and operators
    *   [ ] Parse control flow statements (`if`, `else`, `while`, `for`, `do-while`, `match`)
    *   [ ] Parse function definitions and calls
    *   [X] Write unit tests for the parser
*   [ ] **Task 4: Bytecode VM**
    *   [X] Design bytecode instruction set
    *   [X] Implement a simple stack-based VM
    *   [X] Implement instruction fetching and decoding
    *   [X] Write unit tests for the VM
*   [ ] **Task 5: Compiler (AST to Bytecode)**
    *   [X] Traverse AST to generate bytecode
    *   [ ] Implement symbol table for variable management
    *   [X] Compile expressions and statements
    *   [X] Write unit tests for the compiler
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
    *   [X] Create a CLI for running Nota scripts
    *   [ ] Add support for file input and interactive mode
*   [ ] **Task 14: Debugger**
    *   [ ] Implement a simple debugger
    *   [ ] Support for breakpoints and stepping
