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
