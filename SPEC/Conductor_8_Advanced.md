# Conductor 8: Advanced Features

## Goal
Implement advanced control flow and event handling mechanisms: Event Delegation and For Loops.

## Features

### 1. Event Delegation
Allow a component to forward events to another component (parent or ID).
```nota
Rect {
    id: container
    Rect {
        delegate [onClick, onHover] for parent
        delegate [onClick] for container
    }
}
```
**Implementation:**
- AST: `DelegateNode`
- CodeGen: Inject JS logic to bind events on the current element and dispatch them on the target.

### 2. For Loops
Support iterating over lists to generate dynamic content.
```nota
for (item in list) {
    Text { text: item.name }
}
```
**Implementation:**
- AST: `ForNode`
- CodeGen:
  - This requires runtime generation if the list is dynamic.
  - For the prototype, we can implement a `NotaListRenderer` in the runtime JS.
  - The `for` body will be compiled into a JS template/function that returns the HTML string or DOM elements.

## Strategy
1.  **Delegate:** Parse `delegate` keyword, list of events, and target. Generate JS: `this.el.addEventListener('click', () => target.dispatchEvent(new Event('click')))`.
2.  **For Loop:** Parse `for (var in list) { body }`. Generate JS: `list.forEach(var => { ... render body ... })`. This is complex because the body contains Nota components.
    - *Simplification for Prototype:* We can treat the body of the for loop as a "Template".
    - The Compiler will generate a JS function `render_for_X(item)` that constructs the DOM elements for the body.
    - The Runtime will call this function.
