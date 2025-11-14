# Nota Language Specification

This document defines the grammar and semantics of the Nota programming language.

## Phase 1: Basic Lexical Elements

### Tokens

The Nota language will be composed of the following tokens:

- **Numbers:** Integers and floating-point numbers.
  - `123`
  - `3.14`
- **Identifiers:** Names for variables and functions, starting with a letter or underscore, followed by letters, numbers, or underscores.
  - `my_variable`
  - `_another_var`
  - `func1`
- **Operators:** Basic arithmetic operators.
  - `+` (plus)
  - `-` (minus)
  - `*` (multiply)
  - `/` (divide)
- **Punctuation:** Parentheses for grouping expressions.
  - `(` (left parenthesis)
  - `)` (right parenthesis)
- **Whitespace:** Spaces, tabs, and newlines are used to separate tokens and will be ignored by the parser.
- **End of File (EOF):** A special token to signify the end of the input stream.
