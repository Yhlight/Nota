# Conductor 1: Genesis (Core Compiler)

## Objective
Establish the compiler infrastructure and successfully compile a minimal Nota file into a viewing HTML page.

## Scope
1.  **Project Skeleton**:
    - CMake build system with C++20 standard.
    - Google Test integration.
    - Directory structure (`src`, `include`, `tests`).

2.  **Lexer**:
    - Implement a tokenizers to handle:
        - Identifiers (`App`, `Rect`, `width`, `color`)
        - Numbers (`100`, `100.5`)
        - Strings (`"Nota Dashboard"`)
        - Symbols (`{`, `}`, `:`, `;`)
        - Comments (`//`, `/* */`)

3.  **Parser**:
    - Recursive Descent Parser.
    - AST Nodes:
        - `ComponentNode` (type, properties, children)
        - `PropertyNode` (name, value)
    - Parse basic hierarchy:
      ```nota
      App {
          width: 100%
          color: #f0f0f0
          Rect { width: 50; color: red }
      }
      ```

4.  **Code Generator (Backend)**:
    - HTML Emitter:
        - `App` -> `<body class="nota-app">`
        - `Rect` -> `<div class="nota-rect">`
        - `Text` -> `<span class="nota-text">`
    - CSS Emitter:
        - Generate classes like `.nota-rect`.
        - Map `width`, `height`, `color` to CSS properties.
        - Handle units (number -> `px`, string -> literal).

## Deliverables
- `nota` executable.
- `tests` executable passing all unit tests.
- `index.html` output for a sample input.
