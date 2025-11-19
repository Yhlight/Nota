## Nota
Nota是一门通用范畴的解析型语言，Nota将使用基于字节码的虚拟机作为运行时环境，并且带有垃圾回收  
Nota的文件名称使用.nota作为文件扩展名，而字节码文件则是使用.n作为文件扩展名  

nota基于C++17 / C++20开发  
nota语言不需要以分号结尾  
但是nota语言以'\n'作为一个语句的结束  

## 注释
nota支持单行注释和多行注释  
使用//和/*...*/  

## 语句的跨行连接
如果一行以运算符结尾（例如 `+`, `-`, `*`, `/`, `=`, `(`, `[` 等），解析器会自动认为下一行是当前语句的延续，从而支持隐式跨行。

```nota
let a = 10 + 20
    + 30
```

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
++ --
```

## 选择结构
Nota支持2种选择结构  
if...else  
match    

```nota
if 表达式

else if 表达式

else 表达式

end
```


```nota
match 表达式
结果:

结果2:

结果3:

结果4, 结果5，结果6:

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

while 表达式
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

(a: int, b: int) =>

return a + b

end
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

## 未定义值
在Nota中，任意类型都可以被赋值为none，表示未定义  
一旦被赋予none值的变量被不当使用，将发生错误  

## 类型转换
Nota支持类型转换，你只需要像常规的编程语言的强制类型转换一样进行转换  
对于Nota来说，类型转换往往是具有主观性的，这种行为不视为强制  
注意，对于自定义对象，如果需要转换为字符串类型，请实现to_string()方法  

nota通常情况下支持下述的类型转换  
int -> float  隐式转换  
float -> int  隐式转换  // 遵循截断
int -> bool  隐式转换  
bool -> int  隐式转换  
flot -> bool  隐式转换  
bool -> float  隐式转换  

所有的字符串操作都应该显性进行  
string -> int  显式转换  
int -> string  显式转换  
string -> float  显式转换  
float -> string  显式转换  
string -> bool  显式转换  
bool -> string  显式转换  

数组，函数不适用转换  
对象仅支持字符串转换  

```nota
let a: int = (int)10.0
let b: int = (int)"10"
```

## 溢出
Nota与lua一致，采用静默回绕处理溢出值  

## 复制，引用，拷贝
对于基本数据类型，Nota的默认行为是复制，这是不会发生改变的做法  
对于引用数据类型，Nota的默认行为是不可变引用，如果你需要可变引用，你需要使用&操作符  
对于引用数据类型，如果你想要进行拷贝操作，你需要使用*操作符  
注意！let和mut类型的变量能够直接传递给函数的不可变引用参数  
但是只有mut类型的变量能够传递给函数的可变引用参数  
而对于函数的拷贝参数，无论你是let还是mut，都将进行拷贝操作  
对于需要使用*操作符的自定义对象，你必须实现clone()方法  

这些操作应该被用于函数参数  

```nota
func add(a: string, b: &string, c: *string): int
    // a 是不可变引用
    // b 是可变引用
    // c 是拷贝操作
end
```

FFI不需要写入语法规范之中  
## FFI
这是一个常见的现代化做法的FFI接口实现，仅供参考  

### 类型转换器
```cpp
template<typename T>
NotaValue Marshal(T value)
{

}

template<typename T>
T Unmarshal(const NotaValue& value)
{
    
}

template<>
NotaValue Marshal(int value)
{
    return NotaValue::CreateInt(static_cast<long long>(value));
}

template<>
int Unmarshal(const NotaValue& value)
{
    if (value.GetType() != NotaValue::TypeTag::NOTA_INT)
    {
        throw std::runtime_error("Type mismatch: Expected int");
    }
    return static_cast<int>(value.GetInt());
}

// 对象类型往往需要GC句柄
template<>
NotaValue Marshal(const std::string& value)
{
    NotaObject* obj = VM::GetCurrent()->AllocateString(value);
    return NotaValue::CreateObject(obj);
}
```

### 函数包装器
```cpp
template<typename R, typename... Args>
class NativeCaller {
public:
    static NotaValue Call(R (*func)(Args...), const std::vector<NotaValue>& args_from_vm)
    {
        if (args_from_vm.size() != sizeof...(Args))
        {
            throw std::runtime_error("Native function argument count mismatch.");
        }

        return [&]<size_t... Is>(std::index_sequence<Is...>) -> NotaValue
        {
            R result = func(Unmarshal<Args>(args_from_vm[Is])...);
            return Marshal(result);
        }(std::make_index_sequence<sizeof...(Args)>{});
    }
};

template<typename... Args>
class NativeCaller<void, Args...>
{
public:
    static NotaValue Call(void (*func)(Args...), const std::vector<NotaValue>& args_from_vm)
    {
        [&]<size_t... Is>(std::index_sequence<Is...>)
        {
            func(Unmarshal<Args>(args_from_vm[Is])...);
        }(std::make_index_sequence<sizeof...(Args)>{});

        return NotaValue::CreateNull();
    }
};
```

### 注册
```cpp
using NativeFuncPtr = std::function<NotaValue(const std::vector<NotaValue>&)>;

template<typename R, typename... Args>
void RegisterNative(VM* vm, const std::string& name, R (*func)(Args...))
{
    NativeFuncPtr wrapper = [func](const std::vector<NotaValue>& args) -> NotaValue
    {
        try
        {
            return NativeCaller<R, Args...>::Call(func, args);
        } catch (const std::exception& e)
        {
            vm->RaiseError(e.what());
            return NotaValue::CreateNull();
        }
    };

    vm->RegisterName(name, wrapper);
}
```

### 示例
```cpp
int native_add(int a, int b)
{
    return a + b;
}

void native_print_message(std::string msg)
{
    std::cout << "Nota Message: " << msg << std::endl;
}

VM nota_vm;

RegisterNative(&nota_vm, "add", &native_add);
RegisterNative(&nota_vm, "print", &native_print_message);
```

```nota
let result = add(10, 20)
print("Hello from Nota")
```
