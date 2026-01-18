# Nota Language Specification

This document provides a formal specification for the Nota language. It is the single source of truth for Nota's syntax and semantics.

## 1. Introduction

Nota is a declarative, compiled UI language with a syntax similar to QML. It is designed to build modern web interfaces. Nota source code files use the `.nota` extension. The compiler transforms Nota code into HTML, CSS, and JavaScript.

## 2. Lexical Structure

### 2.1. Comments

Nota supports two types of comments:

- **Single-line comments:** Begin with `//` and continue to the end of the line.
- **Multi-line comments:** Begin with `/*` and end with `*/`.

### 2.2. Identifiers

Identifiers are used to name components, properties, and other entities. They must start with a letter or an underscore, and can be followed by letters, numbers, or underscores.

### 2.3. Keywords

The following are reserved keywords in Nota:

- `import`
- `as`
- `package`
- `export`
- `if`
- `else`
- `property`
- `delegate`
- `for`
- `states`
- `State`
- `when`
- `Struct`

### 2.4. Literals

- **Integer:** `123`, `-45`
- **Double:** `1.03`, `3.14159`
- **Boolean:** `true`, `false`
- **String:** `"Hello, World!"`, `'Another string'`

## 3. Basic Components

All Nota components have the following default CSS properties:

```css
box-sizing: border-box;
overflow: hidden;
margin: 0;
padding: 0;
```

### 3.1. `App`

The root component of a Nota application. It compiles to an HTML `<body>` tag with the class `nota-app`.

### 3.2. `Row`

A container that arranges its children horizontally. It compiles to a `<div>` with `display: flex` and `flex-direction: row`.

### 3.3. `Col`

A container that arranges its children vertically. It compiles to a `<div>` with `display: flex` and `flex-direction: column`.

### 3.4. `Rect`

A basic rectangular component. It compiles to a `<div>` with `display: block`.

### 3.5. `Text`

A component for displaying text. It compiles to a `<span>` with `display: block`.

## 4. Component Model

### 4.1. `Item`

`Item` is the base for all components. It can be used to define custom components.

```Nota
Item MyComponent {
    // ... component definition
}
```

When used directly, `Item` compiles to a generic `<div>`.

### 4.2. Properties

Components have properties that can be set.

```Nota
Rect {
    width: 100
    height: 50
    color: "red"
}
```

### 4.3. Property Overriding

Properties of custom components can be overridden when an instance is created.

```Nota
Item MyButton {
    width: 100
    height: 30
}

App {
    MyButton {
        width: 150 // Overrides the default width
    }
}
```

### 4.4. Property Referencing

Properties can reference other properties using the `id` of the component.

```Nota
Rect {
    id: myRect
    width: 200
    height: myRect.width / 2 // height will be 100
}
```

### 4.5. Children

Components can have child components.

```Nota
Row {
    Rect { /* ... */ }
    Text { /* ... */ }
}
```

### 4.6. Accessing Children

Children can be accessed via the `children` property.

```Nota
MyComponent {
    this.children[0].color: "blue" // Modifies the first child
}
```

## 5. Positioning

### 5.1. `x` and `y`

Components can be positioned relative to their parent using the `x` and `y` properties. The parent component must have `position: relative;`.

### 5.2. `position`

The `position` property aligns a component within its parent. It can be used with `x` and `y` for further adjustments.

## 6. Stacking Order

The `index` property controls the stacking order of components.

## 7. Modules

### 7.1. `import`

The `import` statement is used to load other Nota files.

```Nota
import "path/to/file.nota";
import "path/to/file.nota" as MyModule;
```

### 7.2. `package`

The `package` statement is used to group modules into a package.

```Nota
package MyUI;
```

### 7.3. `export`

The `export` keyword makes a component available to other modules.

```Nota
export Item MyComponent { /* ... */ }
```

## 8. Event Handling

### 8.1. Event Properties

Components can have event properties like `onClick` and `onHover`.

```Nota
Rect {
    onClick: "alert('Clicked!');"
}
```

### 8.2. Event Delegation

Events can be delegated to a parent component using the `delegate` keyword.

```Nota
delegate [onClick, onHover] for parent
```

## 9. State Management

### 9.1. `states`

The `states` property is a list of `State` objects.

### 9.2. `State`

A `State` defines a set of property changes that are applied when the state is active.

```Nota
states: [
    State {
        name: "hovered"
        when: HoverEvent
        color: "red"
    }
]
```

## 10. Conditional Rendering

The `if` and `else` keywords can be used for conditional rendering.

```Nota
if (someCondition) {
    Rect { /* ... */ }
} else {
    Text { /* ... */ }
}
```

## 11. Type System

### 11.1. Basic Types

- `int`
- `double`
- `bool`
- `string`
- `void`
- `list`
- `State`

### 11.2. Custom Value Types (`Struct`)

Custom value types can be defined using the `Struct` keyword.

```Nota
export Struct Color {
    int r;
    int g;
    int b;
}
```
