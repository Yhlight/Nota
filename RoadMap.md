# Nota Compiler Development Roadmap

This document outlines the development plan for the Nota compiler, aiming for a professional-grade, production-ready implementation. The development is divided into distinct phases, each focusing on a specific part of the compiler's architecture.

## Phase 1: Core Infrastructure & Lexer

- **Objective:** Establish the foundational components of the compiler and implement the lexical analyzer.
- **Key Tasks:**
    - [x] **Project Setup:**
        - [x] Set up a C++ project using CMake.
        - [x] Integrate a testing framework (e.g., Google Test).
        - [x] Define core data structures and utility functions (e.g., error reporting).
    - [x] **Lexer Implementation:**
        - [x] Define all tokens based on `Nota.md` (e.g., identifiers, keywords, symbols, literals).
        - [x] Implement the lexer to transform Nota source code into a stream of tokens.
        - [x] Add comprehensive unit tests for the lexer, covering all token types and edge cases.

## Phase 2: Parser & Abstract Syntax Tree (AST)

- **Objective:** Implement a parser to build an Abstract Syntax Tree (AST) from the token stream.
- **Key Tasks:**
    - [x] **AST Design:**
        - [x] Define the structure of the AST nodes to represent the Nota language constructs (e.g., components, properties, nested structures).
    - [x] **Parser Implementation:**
        - [x] Implement a parser (e.g., recursive descent or Pratt parser) to consume the token stream and produce an AST.
        - [x] Implement robust error handling and recovery mechanisms within the parser.
    - [x] **Parser Testing:**
        - [x] Write extensive unit tests for the parser, verifying the correctness of the generated AST for various Nota code snippets.

## Phase 3: Semantic Analysis

- **Objective:** Perform semantic checks on the AST to ensure the code is logically correct.
- **Key Tasks:**
    - [ ] **Symbol Table:**
        - [ ] Implement a symbol table to track identifiers, components, and their properties.
    - [ ] **Type Checking:**
        - [ ] Implement type checking for property assignments (e.g., `width` should be a number or percentage).
    - [ ] **Semantic Validation:**
        - [ ] Validate component-specific properties (e.g., `spacing` is only valid for `Row` and `Col`).
        - [ ] Check for undefined components or properties.
        - [ ] Implement rules for property overriding and child access.

## Phase 4: Code Generation (HTML/CSS)

- **Objective:** Traverse the validated AST to generate the final HTML and CSS output.
- **Key Tasks:**
    - [ ] **HTML Generator:**
        - [ ] Implement a code generator that walks the AST to produce the corresponding HTML structure.
        - [ ] Map Nota components (`App`, `Row`, `Col`, `Rect`, `Text`) to their HTML equivalents (`body`, `div`, `span`).
    - [ ] **CSS Generator:**
        - [ ] Implement a CSS generator that creates stylesheets based on the component properties defined in the Nota source.
        - [ ] Handle property mappings (e.g., `width`, `height`, `color`, `spacing` -> `gap`).
        - [ ] Implement logic for relative positioning (`x`, `y`, `position`) and z-indexing (`index`).
    - [ ] **File Output:**
        - [ ] Combine the generated HTML and CSS into a single `.html` file or separate files.

## Phase 5: Advanced Features & Refinement

- **Objective:** Implement advanced language features and optimize the compiler.
- **Key Tasks:**
    - [ ] **Custom Components:**
        - [ ] Fully implement the `Item` component for creating custom, reusable components.
        - [ ] Handle property passing and overriding for custom components.
    - [ ] **CLI (Command-Line Interface):**
        - [ ] Develop a user-friendly CLI for the compiler.
    - [ ] **Performance Optimization:**
        - [ ] Profile and optimize the compiler's performance.
    - [ ] **Documentation:**
        - [ ] Write comprehensive documentation for the Nota language and the compiler.
