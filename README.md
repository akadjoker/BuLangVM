BULangVM is a hybrid programming language that combines elements of Pascal, Python, and C.

# Stack-Based Virtual Machine



## Features

- **Stack-Based Operations**: Utilizes push and pop operations for efficient execution.
- **Lua, C, and Python-Inspired Syntax**: Easy to learn and use for those familiar with these languages.
- **Integration with C++**: Easily register native functions and variables.
- **High Performance**: Capable of running 40,000 processes at 60 FPS.
- **Efficient Scripting**: Designed for high-performance scripting and execution.


#### Registering Functions and Variables

To register a native function or a global variable, use the following methods:

```cpp
// Example of registering a native function
vm.registerFunction("rand", native_rand, 0);

// Example of registering a global variable
int screenWidth = 800;
vm.registerInteger("screenWidth", screenWidth);

static int native_rand(VirtualMachine *vm, int argc, Value *args) {
    vm->push(NUMBER(Random()));
    return 1;
}


```