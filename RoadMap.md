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
    - [ ] Static arrays (`int[10]`)
- [x] **Classes**
    - [x] Class declaration (`class`)
    - [x] Member variables
    - [x] Member functions
    - [x] `this` keyword
    - [x] Constructors (`new`)
    - [x] Object instantiation (default, positional, named)

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
