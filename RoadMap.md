# Nota Project Roadmap

This document outlines the development roadmap for the Nota programming language.

## Phase 1: Core Language Features & Infrastructure

- [x] **Project Setup**
    - [x] Directory Structure (`src`, `include`, `tests`)
    - [x] CMake Build System
    - [x] Python Build Script
    - [x] Test Framework Integration (GoogleTest)
- [x] **Lexer & Parser Basics**
    - [x] Comment Parsing (`//`, `/* */`)
    - [x] Tokenization of basic keywords (`let`, `mut`, `if`, etc.)
    - [x] End-of-statement handling (`\n`)
    - [x] Statement continuation (`+`)
- [x] **Variables & Data Types**
    - [x] Variable declaration (`let`, `mut`)
    - [x] Basic types (`int`, `float`, `bool`)
    - [x] Type annotations
    - [x] String literals
- [x] **Operators**
    - [x] Arithmetic operators (`+`, `-`, `*`, `/`, `%`)
    - [x] Comparison operators (`==`, `!=`, `>`, `<`, etc.)
    - [x] Logical operators (`&&`, `||`, `!`)
    - [x] Assignment operators (`=`, `+=`, etc.)

## Phase 2: Control Flow

- [x] **Conditional Structures**
    - [x] `if...else if...else` statements
    - [x] `match` statements
- [x] **Looping Structures**
    - [x] `while` loops
    - [x] `do-while` loops
    - [x] C-style `for` loops
    - [x] `for-each` loops

## Phase 3: Functions, Scope, and Interpreter

- [x] **Functions**
    - [x] Function declaration and calls (`func`)
    - [x] Return statements
    - [x] Parameter lists with type annotations
- [x] **Lambda Functions**
    - [x] Basic lambda syntax (`=>`)
- [x] **Interpreter**
    - [x] Tree-walk interpreter foundation
    - [x] Expression evaluation
    - [x] Statement execution
- [x] **Scope Management**
    - [x] Global scope
    - [x] Block scope
    - [x] Function scope

## Phase 4: Advanced Data Structures & OOP

- [x] **Arrays**
    - [x] Dynamic arrays (`int[]`)
    - [x] Static arrays (`int[10]`)
- [x] **Classes**
    - [x] Class declaration (`class`)
    - [x] Member variables
    - [x] Member functions
    - [x] `this` keyword
    - [x] Constructors (`new`)
    - [x] Object instantiation (default, positional, named)

## Phase 5: Modules and Packaging

- [x] **Modules**
    - [x] `import` statements
    - [x] Module aliasing (`as`)
    - [x] Namespace resolution (`::`)
- [x] **Packages**
    - [x] `package` declaration

## Phase 6: Bytecode VM

- [x] **Bytecode Generation**
    - [x] Design instruction set
    - [ ] Compile AST to bytecode
        - [x] Logical and comparison operators
        - [ ] Variable declaration and access (`let`, `mut`)
        - [ ] Assignment expressions
- [x] **Virtual Machine**
    - [x] Stack-based execution
    - [x] Instruction dispatch loop
    - [ ] Garbage Collection

## Phase 7: Advanced Language Features

- [ ] **Undefined Values**
    - [ ] `none` keyword and value
- [ ] **Type Conversions**
    - [ ] Implicit and explicit type casting
- [ ] **References and Copying**
    - [ ] `&` operator for mutable references
    - [ ] `*` operator for copying
