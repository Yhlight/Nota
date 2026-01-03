/*
    作者： yinghuolight
    日期：2025/12/31
    版本：Nota-MVP
*/

/*
    作者： yinghuolight
    日期：2026/1/1
    版本：Nota-原型，Item，属性覆写，子控件访问，组件模型
    ，相对位置，xy，position，层级
*/

## Nota
Nota是一门与QML语法高度相似的前端界面语言

Nota使用.nota作为文件扩展名  

nota基于C++20开发，是一门编译型编程语言，最终产物是HTML + CSS代码  

Nota舍弃了HTML的语法，这可能对于前端开发者来说很不友好  

## 注释
nota支持单行注释和多行注释  
使用//和/*...*/  

## 示例代码
```Nota
App {
    width: 100%;
    height: 100%;
    color: #f0f0f0;

    Row {
        id: header;
        width: 100%;
        height: 60;
        color: #333;
        spacing: 20;

        Rect { width: 40; height: 40; radius: 20; color: white; }
        Text { text: "Nota Dashboard"; color: white; }
    }

    Col {
        width: 80%;
        padding: 40;
        spacing: 15;

        Rect { 
            width: 100%; 
            height: 200; 
            color: white; 
            radius: 8;
        }

        Row {
            spacing: 10;
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
        color: red;
        width: 50px;
        height: 50px;
        text: "盒子";
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
    color: red;
    width: 50px;
    height: 50px;
    text: "盒子";

    Rect
    {
        color: red;
        width: 50px;
        height: 50px;
        text: "盒子";
    }
}

App
{
    Box
    {
        color: black;  // 覆写Box的颜色
        // this. 是可选的
        // this.color: black;

        Rect  // 追加一个Rect，而不是覆写第一个Rect
        {
            color: black;
        }
    }
}
```

#### 子控件访问

```nota
Item Box
{
    Rect
    {
        color: red;
        width: 50px;
        height: 50px;
        text: "盒子";
    }

    Rect
    {

    }
}

App
{
    Box
    {
        // 'this.' 是可选的
        children[0].color: "black";  // 覆写
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
    witdh: 100px;
    height: 100px;

    Rect
    {
        width: 10px;
        height: 10px;
        x: parent.width / 2;
        y: parent.height / 2;
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
        position: left top;
        x: 50;
        y: 50;
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
        index: 2;  // 在父亲上
    }

    Item
    {
        index: -1;  // 在父亲下
    }
}
```

预期的父组件的CSS中必然拥有position: relative;  

### 属性

Nota现在支持下述属性  
width, height  =>  支持数字(px，不带单位的数字都是px)或百分比  
spacing  =>  仅支持数字，仅在Row和Col中有效，映射为gap  
padding  =>  内边距，支持数字(px)或百分比。
text  =>  容器的文本，Text也可以使用  
class，id  
x, y  
index

视觉效果  
color  =>  背景颜色  
border: 边框线（如 1 solid black）
radius: 圆角