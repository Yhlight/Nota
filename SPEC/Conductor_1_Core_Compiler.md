# Conductor 1: Core Compiler Infrastructure

## Objectives
Initialize the Nota compiler project and implement the frontend (Lexer/Parser) capable of processing the basic structural syntax of Nota.

## Tasks

### 1.1 Build System Setup
*   Initialize a CMake project (minimum version 3.20).
*   Define project structure:
    *   `src/`: Compiler source code.
    *   `include/`: Header files.
    *   `tests/`: Unit tests.
*   Integrate Google Test.

### 1.2 Lexer Implementation
*   Create `Lexer` class.
*   Define `Token` structure (Type, Value, Location).
*   Token Types:
    *   Identifiers (e.g., `App`, `width`, `color`).
    *   Keywords (`Item`, `property`, `import`, etc. - start with a minimal set).
    *   Literals: String (`"text"`), Number (`100`, `10.5`, `#f0f0f0` hex colors).
    *   Symbols: `{`, `}`, `:`, `;`, `.`, `[`, `]`, `(`, `)`, `,`.
    *   Operators: `+`, `-`, `*`, `/`, `=`.
*   Support comments (`//` and `/* ... */`).

### 1.3 AST Definition
*   Define base `ASTNode` class.
*   Define node types:
    *   `ComponentNode`: Represents `Type { ... }`.
    *   `PropertyNode`: Represents `name: value`.
    *   `ValueNode`: Base for literal values.
*   Implement `Visitor` pattern for AST traversal (useful for later stages).

### 1.4 Parser Implementation
*   Create `Parser` class.
*   Implement Recursive Descent Parsing logic.
*   Support basic Component nesting:
    ```nota
    Type {
        Property: Value
        NestedType { }
    }
    ```
*   Error handling: Report syntax errors with line/column numbers.

### 1.5 CLI Entry Point
*   Create `main.cpp`.
*   Accept input file path argument.
*   Run Lexer -> Parser.
*   Debug output: Print the AST structure to stdout (or a textual representation) to verify correctness.

## Acceptance Criteria
*   The project builds successfully with CMake.
*   Unit tests for Lexer cover all token types.
*   Unit tests for Parser cover nested components and properties.
*   A sample `.nota` file can be parsed into an AST without errors, and the AST structure can be visualized/printed.
