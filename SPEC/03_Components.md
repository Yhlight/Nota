# Conductor 3: The Component Model

## Objective
Implement the "Factory" pattern of Nota, allowing users to define new components based on existing ones (`Item`).

## Scope
1.  **Custom Components (`Item`)**:
    - Parsing `Item Name { ... }`.
    - Registering `Name` as a new type in the Symbol Table.
    - Instantiating `Name` inside other components.

2.  **Property Mechanics**:
    - **Overriding**: Setting a property in an instance overwrites the default from the definition.
    - **Reference**: `id` attribute. Accessing properties via `id.prop` or `parent.prop`.
    - **This**: `this` keyword.

3.  **Hierarchy Access**:
    - `parent`: Accessing the parent's properties.
    - `children`: Accessing child components by index.
    - `index`: Z-index control.

## Technical Details
- The compiler needs a Symbol Table to track component definitions.
- Property lookup needs to handle scope resolution (Local -> ID -> Parent).
- "Property non-penetration principle": Ensure properties don't leak unexpectedly.

## Deliverables
- Compiler support for defining and using `Item`.
- Correct handling of `id` and property references in generated JS/CSS.
