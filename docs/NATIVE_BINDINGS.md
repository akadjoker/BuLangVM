# BuLang Native Bindings Guide

Este documento explica como criar bindings de C++ para BuLang VM.

## 1. Structs Nativos (POD - Plain Old Data)

Structs nativos são tipos simples sem métodos, apenas campos de dados.

### Definição C++

```cpp
struct Vector2 {
    float x;
    float y;
};
```

### Construtor

```cpp
static void vector2_ctor(Interpreter* vm, void* buffer, int argc, Value* args) {
    Vector2* vec = static_cast<Vector2*>(buffer);
    vec->x = (argc > 0) ? args[0].asNumber() : 0.0f;
    vec->y = (argc > 1) ? args[1].asNumber() : 0.0f;
}
```

### Registro

```cpp
NativeStructDef* vec2 = vm.registerNativeStruct(
    "Vector2",              // Nome em BuLang
    sizeof(Vector2),        // Tamanho da struct
    vector2_ctor,           // Construtor
    nullptr,                // Destrutor (nullptr para POD)
    false                   // Não persistente
);

// Registrar campos
vm.addStructField(vec2, "x", offsetof(Vector2, x), FieldType::FLOAT);
vm.addStructField(vec2, "y", offsetof(Vector2, y), FieldType::FLOAT);
```

### Uso em BuLang

```bulang
var v = Vector2(10.0, 20.0);
print(v.x);  // 10.0
v.y = 30.0;
```

## 2. Classes Nativas (com ponteiro)

Classes nativas podem ter métodos e propriedades.

### Definição C++

```cpp
class NativeTimer {
public:
    std::chrono::steady_clock::time_point startTime;
    double elapsed;
    bool running;
    
    NativeTimer() : elapsed(0), running(false) {}
    
    void start() {
        startTime = std::chrono::steady_clock::now();
        running = true;
    }
    
    double getElapsed() {
        // ...
    }
};
```

### Construtor

```cpp
static Value timer_constructor(Interpreter* vm, Value* args, int argCount) {
    NativeTimer* timer = new NativeTimer();
    return vm->makeNativeInstance(timer);
}
```

### Destrutor

```cpp
static void timer_destructor(void* ptr) {
    delete static_cast<NativeTimer*>(ptr);
}
```

### Métodos

```cpp
static Value timer_start(Interpreter* vm, void* ptr, Value* args, int argCount) {
    NativeTimer* timer = static_cast<NativeTimer*>(ptr);
    timer->start();
    return Value();  // void
}
```

### Propriedades

```cpp
// Getter (read-only)
static Value timer_get_elapsed(void* ptr) {
    NativeTimer* timer = static_cast<NativeTimer*>(ptr);
    return Value(timer->getElapsed());
}

// Getter/Setter (read-write)
static Value timer_get_running(void* ptr) {
    NativeTimer* timer = static_cast<NativeTimer*>(ptr);
    return Value(timer->running);
}

static void timer_set_running(void* ptr, Value value) {
    NativeTimer* timer = static_cast<NativeTimer*>(ptr);
    timer->running = value.asBool();
}
```

### Registro

```cpp
NativeClassDef* timerClass = vm.registerNativeClass(
    "Timer",                // Nome em BuLang
    timer_constructor,      // Construtor
    timer_destructor,       // Destrutor
    0,                      // argCount
    false                   // Não persistente
);

// Registrar métodos
vm.addNativeMethod(timerClass, "start", timer_start);
vm.addNativeMethod(timerClass, "stop", timer_stop);

// Registrar propriedades
vm.addNativeProperty(timerClass, "elapsed", timer_get_elapsed, nullptr);  // read-only
vm.addNativeProperty(timerClass, "running", timer_get_running, timer_set_running);  // read-write
```

### Uso em BuLang

```bulang
var timer = Timer();
timer.start();
// ... fazer algo ...
timer.stop();
print("Elapsed: " + str(timer.elapsed));
```

## 3. Retornando Instâncias de Classes Nativas

Quando um método precisa retornar uma instância de outra classe nativa já registrada:

```cpp
static Value container_getTimer(Interpreter* vm, void* ptr, Value* args, int argCount) {
    Container* container = static_cast<Container*>(ptr);
    
    // Obter a NativeClassDef do Timer
    NativeClassDef* timerClass = nullptr;
    if (!vm->tryGetNativeClassDef("Timer", &timerClass)) {
        // Error: Timer class not found
        return Value();
    }
    
    // Criar uma NativeClassInstance de Timer
    Value timerValue = vm->makeNativeClassInstance(false);  // false = GC gerencia
    NativeClassInstance* instance = timerValue.asNativeClassInstance();
    instance->klass = timerClass;
    instance->userData = (void*)container->timer;  // Ponteiro para o objeto real
    
    return timerValue;
}
```

### Uso em BuLang

```bulang
var container = Container();
var timer = container.getTimer();  // Retorna instância de Timer
timer.start();
```

## 4. Tipos de Campos (FieldType)

Para structs nativos, os seguintes tipos são suportados:

- `FieldType::BYTE` - uint8_t
- `FieldType::SHORT` - int16_t
- `FieldType::INT` - int32_t
- `FieldType::LONG` - int64_t
- `FieldType::FLOAT` - float
- `FieldType::DOUBLE` - double
- `FieldType::BOOL` - bool

## 5. Exemplo Completo

```cpp
void registerBindings(Interpreter& vm) {
    // 1. Registrar Struct
    auto* vec2 = vm.registerNativeStruct(
        "Vector2", sizeof(Vector2), vector2_ctor, nullptr, false);
    vm.addStructField(vec2, "x", offsetof(Vector2, x), FieldType::FLOAT);
    vm.addStructField(vec2, "y", offsetof(Vector2, y), FieldType::FLOAT);
    
    // 2. Registrar Classe
    auto* timer = vm.registerNativeClass(
        "Timer", timer_ctor, timer_dtor, 0, false);
    vm.addNativeMethod(timer, "start", timer_start);
    vm.addNativeProperty(timer, "elapsed", timer_get_elapsed, nullptr);
}

int main() {
    Interpreter vm;
    
    // IMPORTANTE: Registrar bindings ANTES de compilar
    registerBindings(vm);
    
    // Agora compilar e executar código BuLang
    vm.compile(code, codeLen);
    vm.run();
}
```

## 6. Boas Práticas

1. **Sempre registre bindings ANTES de compilar** - Os bindings nativos precisam estar registrados para que o compilador possa atribuir índices corretos.

2. **Use POD structs para dados simples** - Structs são mais leves e eficientes para tipos de dados simples.

3. **Use classes para objetos com comportamento** - Classes permitem métodos e estado mais complexo.

4. **Gerencie memória corretamente**:
   - Structs POD: não precisam de destrutor
   - Classes: sempre forneça destrutor se alocou memória
   - Objetos retornados: use `false` no `makeNativeClassInstance()` para deixar GC gerenciar

5. **Valide argumentos** - Sempre verifique `argCount` antes de acessar `args[]`.

6. **Trate erros** - Use `return Value()` para nil em caso de erro.

## 7. Integração com Otimização de Globais

**IMPORTANTE**: Classes e structs nativos usam um sistema DIFERENTE das variáveis globais do script:

### Como Funciona

1. **Variáveis do script** (definidas com `var` no código BuLang):
   - Compilador: usa `getOrCreateGlobalIndex()` para atribuir índices sequenciais
   - Runtime: acessa via `globalsArray[index]` - O(1) direto

2. **Classes/Structs nativos** (registrados em C++):
   - Registro: `registerNativeClass/Struct()` coloca no HashMap `globals`
   - Compilador: usa `identifierConstant()` para encontrar no HashMap
   - Runtime: acessa via HashMap `globals.get()` - O(log n) mas raramente usado

### Por Que Não Misturar?

- Nativos são registrados ANTES da compilação
- Variáveis do script são descobertas DURANTE compilação
- Usar índices diferentes evita conflitos
- HashMap para nativos é OK (poucos, raramente acessados)
- Array para variáveis do script é crítico (muitas, acesso frequente)

### Regra de Ouro

✅ **Registre bindings ANTES de compilar**
✅ **Nativos ficam no HashMap** 
✅ **Variáveis do script ficam no Array indexado**

Ver: 
- `libbu/src/interpreter.cpp` - `registerNativeClass()` e `registerNativeStruct()`
- `libbu/src/compiler_statements.cpp` - `namedVariable()` diferencia nativos vs script
- `libbu/src/interpreter_runtime_goto.cpp` - `OP_GET_GLOBAL` verifica HashMap primeiro, depois array
