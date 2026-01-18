# Conductor 3: Component System

## Objectives
Implement the "Item" abstraction, allowing users to define custom reusable components, and support the property inheritance/override model.

## Tasks

### 3.1 Custom Component Definition
*   Update Parser to handle `Item ComponentName { ... }` syntax.
*   Store component definitions in a `SymbolTable` or `ComponentRegistry`.
*   Support instantiating custom components by name within other components.

### 3.2 Symbol Resolution & Scope
*   Implement a `Scope` mechanism.
*   When instantiating a custom component, the compiler must look up its definition.
*   Copy/Template mechanism: When `MyComp` is used, the AST from `MyComp`'s definition is effectively instantiated (or referenced).

### 3.3 Property Overrides
*   Implement logic to merge properties defined in the "Prototype" (custom component definition) with properties defined in the "Instance" (usage).
*   Instance properties take precedence.
*   Handle nested overrides (e.g., overriding a property of a child of the custom component).

### 3.4 ID & References
*   Support `id` property.
*   Implement a lookup mechanism to resolve references like `id.property`.
*   Update Code Generator to bind these references.
    *   *Static Resolution*: If the value is constant known at compile time.
    *   *Dynamic Resolution*: Since Nota targets JS, references might need to become JS variables or reactive bindings if they can change. For this stage, we might assume static layout or generate simple JS variables.

## Acceptance Criteria
*   Can define `Item Box { ... }` and use `Box { color: red }`.
*   The generated HTML reflects the structure of `Box` with the overridden color.
*   Can reference `width: other.width` where `other` is an ID of a sibling/parent.
