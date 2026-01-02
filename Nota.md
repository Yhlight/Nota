# Nota 语言规范

## 1. 简介

**Nota** 是一门为现代 Web 开发设计的声明式前端 UI 语言，其语法与 QML 高度相似。它旨在通过提供一种直观、高效的方式来构建用户界面，简化前端开发流程。

Nota 编译器基于 C++20 构建，可将 `.nota` 文件编译为高性能、优化的 HTML 和 CSS 代码。

### 核心特性

- **声明式语法**：清晰地描述 UI 的结构和状态。
- **组件化**：通过可复用的自定义组件 (`Item`) 构建复杂界面。
- **强大的布局系统**：支持基于 Flexbox 的 `Row` 和 `Col` 布局，以及通过 `x`, `y`, `index` 实现的精确定位。
- **静态类型**：在编译时进行类型检查和属性验证，减少运行时错误。

## 2. 语法基础

### 2.1. 注释

Nota 支持单行和多行注释。

```nota
// 这是一个单行注释

/*
  这是一个
  多行注释
*/
```

### 2.2. 基本结构

一个 Nota 文件通常包含一个根组件（如 `App`）以及一系列可选的自定义组件 (`Item`) 定义。

```nota
// 定义一个可复用的自定义组件
Item CustomButton {
    width: 100;
    height: 40;
    color: "blue";
    radius: 8;
}

// 根组件
App {
    width: "100%";
    height: "100%";

    // 实例化自定义组件
    CustomButton {
        color: "green"; // 覆写属性
    }
}
```

### 2.3. 属性赋值

属性通过 `属性名: 值;` 的形式进行赋值。分号 (`;`) 是可选的。

```nota
Rect {
    width: 200;
    height: 100;
    color: "#ff0000"; // 红色
}
```

## 3. 组件

### 3.1. 内置组件

#### `App`
`App` 是所有 Nota 应用的根组件，它会被编译为 HTML 的 `<body>` 标签。通常用于设置全局样式。

- **可用属性**: `width`, `height`, `color`

#### `Row`
`Row` 是一个使用 Flexbox `row` 方向进行布局的容器。

- **可用属性**: `width`, `height`, `color`, `spacing` (映射为 `gap`)

#### `Col`
`Col` 是一个使用 Flexbox `column` 方向进行布局的容器。

- **可用属性**: `width`, `height`, `color`, `spacing` (映射为 `gap`)

#### `Rect`
`Rect` 是一个通用的矩形块级元素，可用于构建各种视觉元素。

- **可用属性**: `width`, `height`, `color`, `radius`, `border`

#### `Text`
`Text` 用于显示文本内容。它会被编译为 HTML 的 `<span>` 标签。

- **可用属性**: `text`, `color`

### 3.2. 自定义组件 (`Item`)

`Item` 关键字用于定义可复用的自定义组件。这有助于保持代码的模块化和可维护性。

#### 定义 `Item`

```nota
Item Card {
    width: 200;
    height: 150;
    color: "white";
    radius: 12;

    Text {
        text: "这是一个卡片";
    }
}
```

#### 实例化 `Item`

```nota
App {
    // 实例化 Card 组件
    Card {
        // ...
    }
}
```

#### 属性覆写

在实例化自定义组件时，可以覆写其默认属性。

```nota
App {
    Card {
        color: "lightblue"; // 将背景色从 "white" 改为 "lightblue"
    }
}
```

## 4. 动态属性与子组件访问

Nota 支持在运行时通过表达式修改组件属性。

#### `this` 关键字

`this` 关键字指代当前组件实例。

```nota
Rect {
    width: 100;
    height: 100;

    // 无效的示例：Nota 不支持在属性值中直接引用其他属性
    // border: (this.width / 10) + " solid black";

    // 正确的用法是在赋值表达式中使用
}
```

#### 子组件访问

通过 `this.children[index]` 可以访问子组件，并修改其属性。

```nota
Item MyComponent {
    Rect { color: "red"; }   // index 0
    Rect { color: "blue"; }  // index 1
}

App {
    MyComponent {
        // 将第一个子组件 (Rect) 的颜色改为 "green"
        this.children[0].color = "green";
    }
}
```
**注意**: `children` 拼写为 `children` 而不是 `chilren`。

## 5. 布局与定位

### 5.1. 盒子模型

Nota 采用标准的 CSS 盒子模型，所有组件默认设置 `box-sizing: border-box;`。

### 5.2. 相对定位 (`x`, `y`)

通过 `x` 和 `y` 属性可以相对于父组件进行定位。当一个子组件设置了 `x` 或 `y` 时，父组件会自动应用 `position: relative;`，而该子组件则会应用 `position: absolute;`。

```nota
Rect {
    width: 200;
    height: 200;

    Rect {
        width: 50;
        height: 50;
        x: 20; // 距离父组件左侧 20px
        y: 30; // 距离父组件顶部 30px
    }
}
```

### 5.3. 层级 (`index`)

`index` 属性用于控制组件的堆叠顺序，它会被编译为 CSS 的 `z-index`。

```nota
Rect {
    width: 100;
    height: 100;

    Rect {
        width: 50;
        height: 50;
        color: "red";
        index: 2; // 在蓝色矩形之上
    }

    Rect {
        width: 50;
        height: 50;
        color: "blue";
        x: 25;
        y: 25;
        index: 1;
    }
}
```

## 6. 属性参考

| 属性名     | 值类型                     | 适用组件             | 描述                                     | CSS 映射          |
| :--------- | :------------------------- | :------------------- | :--------------------------------------- | :---------------- |
| `width`    | `Number`, `Percentage`     | 所有组件             | 设置组件宽度。                           | `width`           |
| `height`   | `Number`, `Percentage`     | 所有组件             | 设置组件高度。                           | `height`          |
| `color`    | `String` (颜色值)          | 所有组件             | 设置文本颜色 (`Text`) 或背景色 (其他)。    | `color` / `background-color` |
| `spacing`  | `Number`                   | `Row`, `Col`         | 设置子组件之间的间距。                   | `gap`             |
| `padding`  | `Number`, `Percentage`     | 所有组件             | 设置内边距。                             | `padding`         |
| `text`     | `String`                   | `Text`               | 设置文本内容。                           | (内部文本)        |
| `radius`   | `Number`                   | `Rect`               | 设置圆角半径。                           | `border-radius`   |
| `border`   | `String` (如 "1 solid black") | `Rect`               | 设置边框。                               | `border`          |
| `x`        | `Number`                   | 所有组件             | 设置相对于父组件的水平位置。             | `left`            |
| `y`        | `Number`                   | 所有组件             | 设置相对于父组件的垂直位置。             | `top`             |
| `index`    | `Number`                   | 所有组件             | 设置堆叠顺序。                           | `z-index`         |
| `id`       | `String`                   | 所有组件             | 为组件设置一个唯一的 DOM ID。            | `id`              |
