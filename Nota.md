/*
    作者： yinghuolight
    日期：2025/12/31
    版本：Nota-MVP
*/

/*
    作者： yinghuolight
    日期：2026/1/1
    版本：原型，Item，属性覆写，子控件访问，组件模型
    ，相对位置，xy，position，层级
*/

/*
    作者： yinghuolight
    日期：2026/1/2
    版本：属性不穿透原则，模块系统
*/

/*
    作者：yinghuolight
    日期：2026/1/4
    版本：属性引用，按钮，事件响应，事件委派，状态，自定义属性与条件渲染，类型系统
*/

/*
    作者：yinghuolight
    日期：2026/1/11
    版本：导入，模块，包细则，自定义值类型
*/

## Nota
Nota是一门与QML语法高度相似的前端界面语言

Nota使用.nota作为文件扩展名  

nota基于C++20 + JS(运行时库，必要时才使用)开发，是一门编译型编程语言，最终产物是HTML + CSS + JS代码  

Nota舍弃了HTML的语法，这可能对于前端开发者来说很不友好  

## 注释
nota支持单行注释和多行注释  
使用//和/*...*/  

## 示例代码
```Nota
App {
    width: 100%
    height: 100%
    color: #f0f0f0

    Row {
        id: header
        width: 100%
        height: 60
        color: #333
        spacing: 20

        Rect { width: 40; height: 40; radius: 20; color: white; }
        Text { text: "Nota Dashboard"; color: white; }
    }

    Col {
        width: 80%
        padding: 40
        spacing: 15

        Rect { 
            width: 100%
            height: 200
            color: white
            radius: 8
        }

        Row {
            spacing: 10
            Rect { width: 50; height: 50; color: blue; }
            Rect { width: 50; height: 50; color: red; }
        }
    }
}
```

## 基本组件
对于所有的Nota组件，默认具有下述属性  
```css
box-sizing: border-box;
overflow: hidden;
margin: 0;
padding: 0;
```

### App

```Nota
App
{

}
```

=>

```html
<body class="nota-app"></body>
```

### Row

```Nota
Row
{

}
```

=>

```html
<style>
.nota-row
{
    display: flex;
    flex-direction: row;
}
</style>

<div class="nota-row"></div>
```

### Col

```Nota
Col
{

}
```

=>

```html
<style>
.nota-col
{
    display: flex;
    flex-direction: column;
}
</style>

<div class="nota-col"></div>
```

### Rect

```Nota
Col
{

}
```

=>

```html
<style>
.nota-rect
{
    display: block;
}
</style>

<div class="nota-rect"></div>
```

### Text

表示一个可控的文本组件  

```Nota
Text
{

}
```

=>

```html
<style>
.nota-text
{
    display: block;
}
</style>

<span class="nota-text"></span>
```

### 原型

Nota的组件至少会包含五个属性，为type，property，children，this，parent。

其中type为组件的类型，property为组件所包含的属性，而children则是组件所具有的子组件。

this是当前组件的指代，通常用于访问当前组件的属性。

parent顾名思义，父亲。

其中Item是所有组件的基本组件。

Item有两种语义，一种是Item组件，一种是定义一个自定义组件。

```Nota
Item Vec
{
    Item
    {

    }
}
```

### Item

Item表示一个自定义组件。

```Nota
Item Box
{
    Rect
    {
        color: red
        width: 50px
        height: 50px
        text: "盒子"
    }
}

App
{
    Box
    {
        Rect  // 在Box的基础上追加一个Rect
        {

        }
    }
}
```

#### 属性覆写

```nota
Item Box
{
    color: red
    width: 50px
    height: 50px
    text: "盒子"

    Rect
    {
        color: red
        width: 50px
        height: 50px
        text: "盒子"
    }
}

App
{
    Box
    {
        color: black  // 覆写Box的颜色
        // 更标准的写法是 this.color: black

        Rect  // 追加一个Rect，而不是覆写第一个Rect
        {
            color: black
        }
    }
}
```

#### 属性引用

```nota
Item Box
{
    id: box
    color: red
    width: 50px
    height: 50px
    text: "盒子"

    Rect
    {
        color: red
        width: box.width / 2
        height: box.height / 2
        text: "盒子"
    }
}
```

#### 子控件访问

```nota
Item Box
{
    Rect
    {
        color: red
        width: 50px
        height: 50px
        text: "盒子"
    }

    Rect
    {

    }
}

App
{
    Box
    {
        this.children[0].color: "black"  // 覆写
    }
}
```

### 组件模型

对于Nota来说，所有的组件都采用“盒子模型”。

组件会被抽象为  

边框  
内边距  
内容  

Nota不使用外边距  

取而代之的是父组件  

### 相对位置
#### x, y

与QML类似，Nota的组件采用父组件的相对位置来定位子组件的位置。

```Nota
Rect
{
    witdh: 100px
    height: 100px

    Rect
    {
        width: 10px
        height: 10px
        x: parent.width / 2
        y: parent.height / 2
        // 此时左上角点应该与父亲的中心点重合
    }
}
```

预期的父组件的CSS中必然拥有position: relative;  

#### position

除了x，y外，还有position用于控制子组件的位置  

nota有一个专门的位置图  

简单来说，就是自左上角点，将一个组件均分为不等比的9宫格(长边型需要特殊计算)  

使用position: left top;时，子组件左边与上边与父组件左边与下边重合  

可用的参数有left，right，top，buttom，center  

position可以与x，y共同作用  

但是注意！一旦使用了postion，那么x，y的位置是相对于postion定位的那个点的相对位移。

```nota
Item
{
    Item
    {
        position: left top
        x: 50
        y: 50
        // 相对left top 50 50
    }
}
```

### 层级

你可以使用index控制子组件与父组件之间的堆叠关系  

index默认为1  

```nota
Item
{
    Item
    {
        index: 2  // 在父亲上
    }

    Item
    {
        index: -1  // 在父亲下
    }
}
```

预期的父组件的CSS中必然拥有position: relative;  

### 属性不穿透原则

Nota所有组件均不会进行属性的穿透，必须手动设置每一个组件的属性  

### 导入

Nota支持模块化开发，任何一个nota源文件都将视为一个模块  

允许您将代码组织到多个文件中或使用标准库功能，`import`关键字用于加载另一个模块中的组件。

任意import行为都将基于项目本目录进行查找。

#### 语法

`import` 语句接受两种形式：  
1.  **文件路径**: 一个字符串字面量，表示您想要包含的 Nota 文件（`.nota`）的路径。
2.  **模块名**: 一个标识符，表示您想要导入的模块。

```Chtholly
// 导入文件模块
import "path/to/xxx.nota";

// 导入某个模块
import Yhlight_UI;
```

#### 冲突防止

通常情况下，Chtholly默认使用文件名作为模块名。
在冲突时，你可以为模块指定一个别名。

```Chtholly
import "ui.nota" as ui;
import "ui.nota" as ui2;

ui.Item
{

}

ui2.Item
{

}
```

### 包

模块被收录在哪一个包？  

模块与物理结构的目录存在关联。

例如package UI;那么此模块将会被存放在UI文件夹之中。

此时任意放在UI文件夹且具有package UI;顶层语句的nota文件，都将作为一个模块。

例如vec.nota，你可以在其他文件之中使用import UI.vct;

这会导入vec.nota文件之中的所有内容。

要注意import UI.vct时，vct会被隐去，因为它是文件名，此时你需要通过UI.xxx来访问vct包中的内容。

此时你可以直接使用vec.nota文件之中的顶层定义。

也就是包作用域扁平化。

实际上是隐去模块名，也就是import UI.vct; 意思就是导入UI包下的vct.cns文件，但是vec.nota文件是一个无关的干扰项，

此时会隐去vec文件名，将作用域提升为UI  

在冲突时，你需要写出完全的包名(带文件名)。

一般是UI.vec.ite的形式。

```nota
package UI;

Item vec
{

}

/////////////////////
import UI.vec;
import UI.vex;

UI.vec.xxx  // 冲突时需要写出包名
UI.vex.xxx
```

### 导出

你需要使用export来导出组件，以便组件能够被访问  

```nota
export Item Box
{

}

export Item Box2
{

}
```

### 导入，模块，包细则

导入有两种方式，即路径导入与模块导入。

对于路径导入，即"xxx/xxx/xxx.cns"，这一种行为不遵循文件层级，即不遵循package，命名空间就是文件名本身，使用文件名.进行访问。

对于模块导入，即std.ui.button，这一种导入遵循package，即遵循文件层级，命名空间是std.ui，最后一个是模块本身(文件名)，会被隐去，只有发生冲突时，才需要使用std.ui.button进行访问，其他时间使用std.ui访问即可。

### 按钮

```nota
Button
{
    onClick: "alert("CLIKE!!!")"
}
```

### 事件响应

```nota
Rect
{
    onClick: "alert("CLIKE!!!");"
    onHover: {
        console.log("HelloWorld");
        console.log("Hover!!!");
    }
}
```

### 事件委派

```nota
Rect
{
    id: idR

    Rect
    {
        delegate [onClick, onHover] for parent
        delegate [onClick, onHover] for idR  // 委派对象必须存在父级关系

        onClick: "alert("CLIKE!!!")"

        onHover: {
            console.log("HelloWorld");
            console.log("Hover!!!");
        }
    }
}
```

### 状态

```nota
Rect {
    id: mainBox
    width: 100
    height: 100
    color: blue

    states: [
        State {  // 自动状态管理，发生事件自动触发
            name: "down"
            when: ClickEvent
            color: red
        },

        State {
            name: "hov"
            when: HoverEvent
            color: red
        }
    ]

    onClick: {
        state = "down"  // 也可以手动触发，但是推荐自动触发
    }
}
```

### 自定义属性与条件渲染

```nota
Rect
{
    id: box
    property bool isFlag: false
    property bool notDefaultValue

    if (isFlag)  // 表达式的括号可选
    {
        Rect
        {

        }
    }
    else
    {
        Button
        {

        }
    }
}
```

### 类型系统

在Nota之中，存在下述基本类型  

int  整数  
double  浮点数  
bool  布尔值  
string  字符串  
void  空值  
list  列表，使用[]，如果只有一个元素，可以省略[]  
State  状态  

以及下述对象类型  

Item，Rect...  

```nota
Item Box
{
    property int int_value: 123
    property double double_value: 1.03
    property bool bool_value: false
    property string string_value: "HelloWorld"
    property list<Rect> list_value
    property State state: State {
        
    }
    property Item item_value: Item {

    }
}
```

### 自定义值类型

在Nota中，所有的组件的基组件是Item，这意味所有的组件都可以由Item变换而来。

因此，对于值类型来说，也会有对等的逻辑。

现在，你可以使用Struct来定义一个结构类型。

```nota
Struct Color
{
    double r;  // 支持默认值
    double g;
    double b;
}

Item Box
{
    property Color bgc: Color(255, 255, 255)
    property Color bgc: Color {
        255, 255, 255
    }
    property Color bgc: Color {
        r: 255, g: 255, b: 255
    }
}
```

#### 导出的支持

```nota
export Struct xxx
{

}
```

### 属性

Nota现在支持下述属性  
width, height  =>  支持数字(px，不带单位的数字都是px)或百分比  
spacing  =>  仅支持数字，仅在Row和Col中有效，映射为gap  
padding  =>  内边距，支持数字(px)或百分比  
text  =>  容器的文本，Text也可以使用  
id  
x, y  
index  

视觉效果  
color  =>  背景颜色
border: 边框线（如 1 solid black）
radius: 圆角