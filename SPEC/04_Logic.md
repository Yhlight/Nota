# Conductor 4: The Logic

## Objective
Introduce dynamic behavior and type safety to Nota.

## Scope
1.  **Type System**:
    - Implement type checking for `int`, `double`, `bool`, `string`.
    - Support `void` and `list` types.
    - Type inference where possible.

2.  **Expressions**:
    - Parsing and evaluating expressions: `width: parent.width / 2`.
    - Arithmetic (`+`, `-`, `*`, `/`).
    - Logical (`&&`, `||`, `!`).
    - Comparison (`==`, `!=`, `<`, `>`).

3.  **Control Flow**:
    - Conditional Rendering: `if (condition) { ... } else { ... }`.
    - This maps to conditionally generating HTML or using JS to toggle visibility.

4.  **Modules & Imports**:
    - `import "path/to/file.nota"`.
    - `import ModuleName`.
    - Namespace resolution.

## Technical Details
- Expression parsing usually requires a separate parser or precedence climbing.
- `if/else` might require generating JS logic or specific template structures.
- Imports require a file system reader and dependency graph resolution.

## Deliverables
- Support for complex expressions in property values.
- Conditional rendering support.
- Multi-file compilation support.
