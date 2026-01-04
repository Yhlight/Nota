# Nota Compiler Development Roadmap

This document outlines the planned development phases for the Nota compiler, aiming to create a production-ready tool that translates `.nota` files into HTML and CSS.

## Phase 1: Project Scaffolding and Core Components (Foundation)

- **Objective:** Establish a robust and scalable project structure.
- **Tasks:**
    1.  Initialize a C++ project using CMake.
    2.  Define the directory structure (`src`, `include`, `tests`, `app`).
    3.  Create the main entry point for the `nota-compiler` executable.
    4.  Integrate the GoogleTest framework for unit testing.
    5.  Implement a basic command-line interface (CLI) to accept a source file and an output destination.

## Phase 2: Lexical and Syntactic Analysis (Parsing the Source)

- **Objective:** Convert Nota source code into a structured, in-memory representation (AST).
- **Tasks:**
    1.  **Lexer (Tokenizer):**
        - Implement a lexer to scan `.nota` source and produce a stream of tokens (e.g., identifiers, keywords, symbols, literals).
    2.  **Abstract Syntax Tree (AST):**
        - Define C++ classes/structs to represent the grammatical structure of Nota code (e.g., `ComponentNode`, `PropertyNode`, `StringLiteralNode`).
    3.  **Parser:**
        - Implement a recursive descent parser to consume the token stream and build an AST.
        - Start with basic syntax: `Component { Component { ... } }`.
        - Progress to parsing properties: `width: 100`, `color: "#fff"`.

## Phase 3: Semantic Analysis (Understanding the Code)

- **Objective:** Validate the AST for correctness and gather information for code generation.
- **Tasks:**
    1.  **Symbol Table:**
        - Implement a symbol table to manage identifiers (`id`), component definitions, and their scopes.
    2.  **Type Checking:**
        - Traverse the AST to enforce Nota's type system.
        - Validate property assignments (e.g., `width` must be a number or percentage).
        - Resolve references to other components or properties (e.g., `parent.width`).

## Phase 4: Code Generation (Creating the Output)

- **Objective:** Translate the validated AST into functional HTML and CSS.
- **Tasks:**
    1.  **AST Visitor/Walker:**
        - Create a code generator class that traverses the final AST.
    2.  **HTML Generation:**
        - Map Nota components (`App`, `Row`, `Rect`) to their corresponding HTML tags (`body`, `div`, `span`).
        - Handle component nesting to produce a valid HTML document structure.
    3.  **CSS Generation:**
        - Generate unique CSS class names for component instances to apply specific styles.
        - Translate Nota properties (`width`, `color`, `padding`) into CSS rules.
        - Implement logic for complex properties like `spacing` (becomes `gap`) and relative positioning (`x`, `y`).

## Phase 5: Advanced Language Features

- **Objective:** Implement the full feature set of the Nota language.
- **Tasks:**
    1.  **Module System:** Implement `import`, `package`, and `export` functionality.
    2.  **Custom Components:** Handle `Item` definitions, instantiation, and property overriding.
    3.  **Expressions & References:** Parse and translate expressions in property values (e.g., `box.width / 2`).
    4.  **Events & State Management:** Generate the necessary JavaScript to handle `onClick`, `states`, and other dynamic features.
    5.  **Conditional Rendering:** Implement `if/else` logic, likely requiring a client-side JavaScript runtime.

## Phase 6: Tooling and Production Readiness

- **Objective:** Polish the compiler into a professional, user-friendly tool.
- **Tasks:**
    1.  **Error Reporting:** Implement clear and precise error messages, including file names, line numbers, and column numbers.
    2.  **Optimization:** Add options for minifying the output HTML, CSS, and JavaScript.
    3.  **Build System:** Refine the build process for cross-platform compilation and distribution.
    4.  **Documentation:** Write comprehensive documentation for both users and contributors.
