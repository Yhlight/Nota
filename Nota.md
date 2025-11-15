## Nota
Nota是一门通用范畴的解析型语言，Nota将使用基于字节码的虚拟机作为运行时环境  
Nota的文件名称使用.nota作为文件扩展名，而字节码文件则是使用.n作为文件扩展名  

nota基于C++17 / C++20开发  
nota语言不需要以分号结尾  

## 变量
Nota使用动态的类型，为此Nota使用let和mut关键字来声明变量  

你可以使用let来声明一个不可变变量  
```nota
let a = 10
a = 20  // 错误
```

你可以使用mut来声明一个可变变量  
```nota
mut a = 10
a = 20  // 正确
```

## 数据类型与类型注解
Nota拥有3种基本类型  
int，float，bool  

以及数种高级数据类型  
例如字符串(string)，数组(int[]，int[10])，函数function，类(className)  

```nota
let a: int = 10
let b: float = 10.0
let c: bool = true
let d: string = "hello world"
let e: int[] = [1, 2, 3]  // 动态数组
let f: int[10] = [1, 2, 3]  // 静态数组
let g: function = add  // 函数
let h: className = Person{}  // 类
```

## 运算符
Nota支持标准的运算符  

```nota
+ - * / %
== != > >= < <= !
& | ^ ~ << >>
= += -= *= /= %=
&& ||
```

## 选择结构
Nota支持2种选择结构  
if...else  
match    

```nota
if 表达式

end

else if 表达式

end

else 表达式

end
```

```nota
match (表达式)
结果:

end

结果2:

end

结果3:

end

结果4, 结果5，结果6:

end

_:
end
```

## 循环结构
Nota支持4种循环结构  
while  
for  
for-each  
do-while  

```nota
while 表达式

end
```

```nota
for i = 0; i < 10; i++  // C样式，这不是现代化的问题，而是灵活性的考虑

end
```

```nota
for let/mut i : 容器  // 默认let，即不可变，可以不写，也可以显性写出来

end
```

```nota
do

while 表达式 end
```

## 函数
Nota支持函数声明与调用  

```nota
func add(a: int, b: int): int
    return a + b
end

add(1, 2)
```

## lambda函数
nota支持lambda函数  

```nota
let add = (a: int, b: int) => a + b
```

## 类
Nota支持类声明与实例化  

```nota
class Person
    name: string
    age: int

    func new(name: string, age: int)  // 构造函数是可选的，nota支持按位填入
        this.name = name
        this.age = age
    end

    func getName(): string
        return this.name
    end
end

// 三种初始化方式
let person = Person{}  // 默认初始化
let person = Person{"张三"}  // 按位填入
let person = Person{name: "张三", age: 18}  // 按名称填入

person.getName()
```

## 模块
nota使用import关键字来引入模块  

import moduleName / "path"  // 引入模块，优先找官方模块，没有就递归当前项目目录  
在冲突时，你需要使用moduleName::localName来使用模块中的内容  

```
import moduleName / "path" as aliasName

func add(a: int, b: int): int
    return a + b
end

aliasName::add(1, 2)  // 使用模块的add函数
```

## 包
模块被收录在哪一个包？  

```nota
// 例如这个是数学模块
package packageName

func add(a: int, b: int): int
    return a + b
end

/////////////////////
import packageName::math
import packageName2::math

packageName::math::add(1, 2)  // 冲突时需要这样使用
```
