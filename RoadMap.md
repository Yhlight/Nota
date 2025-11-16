# Nota Project Roadmap

This document outlines the development roadmap for the Nota programming language.

## Phase 1: Core Language Features & Infrastructure

- [ ] **Project Setup**
    - [x] Directory Structure (`src`, `include`, `tests`)
    - [ ] CMake Build System
    - [ ] Python Build Script
    - [ ] Test Framework Integration (GoogleTest)
- [ ] **Lexer & Parser Basics**
    - [ ] Comment Parsing (`//`, `/* */`)
    - [ ] Tokenization of basic keywords (`let`, `mut`, `if`, etc.)
    - [ ] End-of-statement handling (`\n`)
    - [ ] Statement continuation (`+`)
- [ ] **Variables & Data Types**
    - [ ] Variable declaration (`let`, `mut`)
    - [ ] Basic types (`int`, `float`, `bool`)
    - [ ] Type annotations
    - [ ] String literals
- [ ] **Operators**
    - [ ] Arithmetic operators (`+`, `-`, `*`, `/`, `%`)
    - [ ] Comparison operators (`==`, `!=`, `>`, `<`, etc.)
    - [ ] Logical operators (`&&`, `||`, `!`)
    - [ ] Assignment operators (`=`, `+=`, etc.)

## Phase 2: Control Flow

- [ ] **Conditional Structures**
    - [ ] `if...else if...else` statements
    - [ ] `match` statements
- [ ] **Looping Structures**
    - [ ] `while` loops
    - [ ] `do-while` loops
    - [ ] C-style `for` loops
    - [ ] `for-each` loops

## Phase 3: Functions and Scope

- [ ] **Functions**
    - [ ] Function declaration and calls (`func`)
    - [ ] Return statements
    - [ ] Parameter lists with type annotations
- [ ] **Lambda Functions**
    - [ ] Basic lambda syntax (`=>`)
- [ ] **Scope Management**
    - [ ] Global scope
    - [ ] Function scope
    - [ ] Block scope

## Phase 4: Advanced Data Structures & OOP

- [ ] **Arrays**
    - [ ] Dynamic arrays (`int[]`)
    - [ ] Static arrays (`int[10]`)
- [ ] **Classes**
    - [ ] Class declaration (`class`)
    - [ ] Member variables
    - [ ] Member functions
    - [ ] `this` keyword
    - [ ] Constructors (`new`)
    - [ ] Object instantiation (default, positional, named)

## Phase 5: Modules and Packaging

- [ ] **Modules**
    - [ ] `import` statements
    - [ ] Module aliasing (`as`)
    - [ ] Namespace resolution (`::`)
- [ ] **Packages**
    - [ ] `package` declaration

## Phase 6: Bytecode VM

- [ ] **Bytecode Generation**
    - [ ] Design instruction set
    - [ ] Compile AST to bytecode
- [ ] **Virtual Machine**
    - [ ] Stack-based execution
    - [ ] Instruction dispatch loop
    - [ ] Garbage Collection (if needed)
