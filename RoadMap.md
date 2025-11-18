# RoadMap

## 核心
- [x] C++项目结构
- [x] CMake构建系统
- [x] Python构建脚本

## 解析器 (Front-End)
- [x] Tokenizer (Lexer)
- [x] AST (基础结构)
- [x] 表达式解析 (一元/二元运算符, 括号)
- [x] 语句解析
  - [x] 变量声明 (`let`/`mut`)
  - [x] Print 语句 (用于调试)
  - [ ] If-Else 语句
  - [ ] While 循环
  - [ ] For 循环
  - [ ] 函数定义
  - [ ] 类定义
- [ ] 类型注解解析

## 虚拟机 (Back-End)
- [x] 字节码 (基础结构, `Chunk`, `OpCode`)
- [x] 虚拟机 (基础结构, 栈, `OP_CONSTANT`, `OP_RETURN`, `OP_NEGATE`)
- [x] 字节码编译器
  - [x] 表达式编译
  - [x] 语句编译
    - [x] 变量声明编译
    - [x] Print 语句编译
- [x] 虚拟机执行
  - [x] 全局变量
  - [x] Print 语句执行
  - [ ] 局部变量
  - [ ] 控制流 (跳转指令)
  - [ ] 函数调用
  - [ ] 类与实例
- [ ] 垃圾回收

## 标准库
- [ ] 核心库
- [ ] 数学库
- [ ] IO库
