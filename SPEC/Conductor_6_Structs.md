# Conductor 6: Structs and Custom Types

## Goal
Implement user-defined value types (`Struct`) to allow grouping of related data, as described in `Nota.md`.

## Features

### 1. Struct Definition
Support the `Struct` keyword to define a new type with fields.
```nota
Struct Color {
    double r;
    double g;
    double b;
}
```

### 2. Struct Instantiation
Support creating instances of structs using syntax similar to function calls or object initialization.
```nota
property Color bgc: Color(255, 255, 255)
// or
property Color bgc: Color { r: 255, g: 255, b: 255 }
```

### 3. Usage
Structs can be used as types for properties.
```nota
Item Box {
    property Color myColor
}
```

## Implementation Details

### AST
- `StructDefinitionNode`: Stores the name and fields of the struct.
- `StructInstantiationNode`: Represents the creation of a struct instance.

### Parser
- `parseStruct()`: Handles top-level `Struct` definitions.
- `parsePrimary()`: Needs to distinguish between `FunctionCall` (if any), `StructInstantiation`, or simple `Identifier`.
  - Since `Color(...)` looks like a function call, and `Nota` doesn't have general functions yet, we can treat `Identifier (...)` as an instantiation or call.

### CodeGen
- For the MVP, Structs will be transpiled to JavaScript Objects (JSON-like structures) to be compatible with the runtime.
- If a Struct is used in a CSS property (like `color`), we might need a `toString()` mechanism or manual mapping. For this Conductor, we will focus on the structure and JS generation, not automatic CSS color conversion (unless trivial).
