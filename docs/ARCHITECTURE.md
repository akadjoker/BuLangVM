# 🏗️ BuLangVM - Arquitetura e Funcionamento

## 📋 Visão Geral

A BuLangVM é uma máquina virtual baseada em bytecode com:
- **Compiler** single-pass (lexer → parser → bytecode)
- **Interpreter** com execução por goto/switch
- **Garbage Collector** mark-and-sweep
- **Sistema de processos** cooperativos (fibers)
- **Bindings nativos** para Raylib e outras libs

---

## 🔄 Fluxo de Execução

```
Código BuLang (.bu)
    ↓
┌─────────────┐
│   LEXER     │ → Tokens (TOKEN_VAR, TOKEN_IF, etc)
└─────────────┘
    ↓
┌─────────────┐
│  COMPILER   │ → Bytecode (OP_ADD, OP_CALL, etc)
└─────────────┘
    ↓
┌─────────────┐
│ INTERPRETER │ → Execução + Runtime
└─────────────┘
```

---

## 📦 Componentes Principais

### 1️⃣ **Lexer** (`libbu/src/lexer.cpp`)

**Função:** Transforma código-fonte em tokens

**Exemplo:**
```bulang
var x = 10;
```

**Tokens gerados:**
```
TOKEN_VAR "var"
TOKEN_IDENTIFIER "x"
TOKEN_EQUAL "="
TOKEN_INT "10"
TOKEN_SEMICOLON ";"
```

**Recursos especiais:**
- Strings verbatim: `@"texto\n literal"`
- Unicode/emojis: `"😀 olá"`
- Escape sequences: `\n`, `\t`, `\uXXXX`

---

### 2️⃣ **Compiler** (`libbu/src/compiler*.cpp`)

**Função:** Converte tokens em bytecode

**Arquitetura:**
- **Pratt Parser** (precedência de operadores)
- **Single-pass** (uma passagem)
- **Scope tracking** (variáveis locais vs globais)

**Divisão:**
- `compiler.cpp` - Core, regras de parsing
- `compiler_expressions.cpp` - Expressões (binary, unary, call, etc)
- `compiler_statements.cpp` - Statements (if, while, for, class, def, etc)

**Exemplo de compilação:**
```bulang
var x = 5 + 3;
```

**Bytecode gerado:**
```
OP_CONSTANT 5      # Push 5
OP_CONSTANT 3      # Push 3
OP_ADD             # Pop 2, add, push result
OP_SET_GLOBAL 0    # Set global var[0] = result
```

**Tabelas importantes:**
- `globals_` - Variáveis globais (indexed array)
- `constants_` - Pool de constantes
- `locals_` - Variáveis locais (stack-based)

---

### 3️⃣ **Interpreter** (`libbu/src/interpreter*.cpp`)

**Função:** Executa bytecode

**Arquitetura:**
- **Stack-based VM** (pilha de valores)
- **Dispatch por goto** (otimizado) ou switch
- **Fibers** (processos cooperativos)

**Arquivos:**
- `interpreter.cpp` - Setup, GC, helpers
- `interpreter_runtime_goto.cpp` - Loop principal (goto)
- `interpreter_runtime_switch.cpp` - Loop alternativo (switch)
- `interpreter_stack.cpp` - Operações de pilha
- `interpreter_process.cpp` - Sistema de processos

**Exemplo de execução:**
```
Pilha inicial: []
OP_CONSTANT 5   → [5]
OP_CONSTANT 3   → [5, 3]
OP_ADD          → [8]
OP_SET_GLOBAL 0 → [] (8 guardado em global[0])
```

---

### 4️⃣ **Sistema de Valores** (`libbu/include/value.hpp`)

**Value** é uma tagged union que pode ser:

```cpp
enum ValueType {
    NIL, BOOL, INT, FLOAT, DOUBLE,
    STRING, ARRAY, MAP, BUFFER,
    FUNCTION, CLOSURE, CLASS, STRUCT,
    PROCESS, POINTER, ...
}
```

**Otimizações:**
- Inteiros: 32-bit direto
- Floats: 32-bit direto
- Strings: Pointer para StringPool
- Arrays/Maps: Pointer para heap object

---

### 5️⃣ **Garbage Collector** (`libbu/src/gc.cpp`)

**Algoritmo:** Mark-and-Sweep

**Fases:**
1. **Mark** - Marca todos objetos alcançáveis
   - Stack de fibers
   - Globais
   - Upvalues abertos
   - Closures ativas

2. **Sweep** - Liberta objetos não-marcados
   - Percorre linked list de objetos
   - Chama destructors
   - Liberta memória

**Quando dispara:**
- Alocação de novo objeto
- Se `totalAllocated > GC_THRESHOLD`

---

### 6️⃣ **Sistema de Processos** (`process`)

**Conceito:** Cooperativo multi-tasking (green threads)

**Hierarquia:**
```
Process (processo pai)
  ├─ Fiber 1 (main)
  ├─ Fiber 2 (criada com fiber { ... })
  └─ Fiber 3
```

**Scheduling:**
- Round-robin
- `yield` cede controle
- `frame` executa N processos por frame

**Estados de Fiber:**
- `RUNNING` - Executando
- `WAITING` - Suspensa
- `DONE` - Terminada

---

### 7️⃣ **Bindings Nativos** (`BuRay/`, `natives/`)

**Como funcionam:**

1. **Definir classe nativa** (C++):
```cpp
NativeClassDef* textureDef = interpreter->registerNativeClass("Texture");
```

2. **Registar métodos** (C++):
```cpp
textureDef->registerMethod("load", texture_load);
```

3. **Implementar método** (C++):
```cpp
int texture_load(Interpreter* vm, void* userData, int argCount, Value* args) {
    const char* path = args[0].asStringChars();
    Texture2D tex = LoadTexture(path);
    // ... guardar em userData
    return 0; // sem retorno
}
```

4. **Usar em BuLang:**
```bulang
var tex = Texture();
tex.load("sprite.png");
```

**Estruturas nativas:**
- `NativeClassDef` - Definição da classe
- `NativeClassInstance` - Instância (userData = ponteiro C)
- `NativeStructDef` - Struct nativo (Vector2, Color, etc)

---

## 🗂️ Estrutura de Dados

### **StringPool** (`libbu/include/string.hpp`)
- Cache global de strings
- Evita duplicação
- Strings são interned (únicas)

### **Arena Allocator** (`libbu/include/arena.hpp`)
- Alocação rápida de objetos pequenos
- Bloco de 64KB
- Fallback para malloc em objetos grandes

### **HashMap/OrderedMap**
- Hash tables para maps e lookups rápidos
- Usado em métodos de classes/structs

---

## 📝 Opcodes Principais

### Controlo de Fluxo
```
OP_JUMP           - Salto incondicional
OP_JUMP_IF_FALSE  - Salto condicional
OP_LOOP           - Loop para trás
```

### Stack
```
OP_POP            - Remove topo
OP_DUP            - Duplica topo
OP_SWAP           - Troca 2 do topo
```

### Variáveis
```
OP_GET_GLOBAL     - Ler global
OP_SET_GLOBAL     - Escrever global
OP_GET_LOCAL      - Ler local
OP_SET_LOCAL      - Escrever local
```

### Operações
```
OP_ADD, OP_SUB, OP_MUL, OP_DIV
OP_EQUAL, OP_LESS, OP_GREATER
OP_AND, OP_OR, OP_NOT
```

### Chamadas
```
OP_CALL           - Chamar função
OP_INVOKE         - Chamar método
OP_RETURN         - Retornar
OP_CLOSURE        - Criar closure
```

### Objetos
```
OP_NEW_ARRAY      - []
OP_NEW_MAP        - {}
OP_NEW_BUFFER     - @(size, type)
OP_NEW_STRUCT     - MyStruct()
OP_NEW_CLASS      - MyClass()
```

---

## 🎯 Exemplo Completo

### Código BuLang:
```bulang
def soma(a, b) {
    return a + b;
}

var resultado = soma(5, 3);
print(resultado);
```

### 1. Lexer Output:
```
TOKEN_DEF, TOKEN_IDENTIFIER("soma"), TOKEN_LPAREN,
TOKEN_IDENTIFIER("a"), TOKEN_COMMA, TOKEN_IDENTIFIER("b"),
TOKEN_RPAREN, TOKEN_LBRACE,
TOKEN_RETURN, TOKEN_IDENTIFIER("a"), TOKEN_PLUS, TOKEN_IDENTIFIER("b"),
TOKEN_SEMICOLON, TOKEN_RBRACE, ...
```

### 2. Bytecode (função soma):
```
Função: soma (2 params)
0000  OP_GET_LOCAL 0      # a
0002  OP_GET_LOCAL 1      # b
0004  OP_ADD
0005  OP_RETURN
```

### 3. Bytecode (main):
```
0000  OP_CLOSURE 0        # Define função soma
0002  OP_SET_GLOBAL 0     # soma = função
0004  OP_GET_GLOBAL 0     # Carrega soma
0006  OP_CONSTANT 5       # arg 1
0008  OP_CONSTANT 3       # arg 2
0010  OP_CALL 2           # Chama com 2 args
0012  OP_SET_GLOBAL 1     # resultado = retorno
0014  OP_GET_GLOBAL 2     # print
0016  OP_GET_GLOBAL 1     # resultado
0018  OP_CALL 1           # print(resultado)
```

### 4. Execução:
```
Stack: []
→ OP_CLOSURE 0      Stack: [<função soma>]
→ OP_SET_GLOBAL 0   Stack: [], globals[0] = <função soma>
→ OP_GET_GLOBAL 0   Stack: [<função soma>]
→ OP_CONSTANT 5     Stack: [<função soma>, 5]
→ OP_CONSTANT 3     Stack: [<função soma>, 5, 3]
→ OP_CALL 2         Stack: [] (chama função, nova frame)
  
  Dentro de soma():
  Stack: [5, 3] (params)
  → OP_GET_LOCAL 0  Stack: [5, 3, 5]
  → OP_GET_LOCAL 1  Stack: [5, 3, 5, 3]
  → OP_ADD          Stack: [5, 3, 8]
  → OP_RETURN       Return: 8

→ Volta ao main    Stack: [8]
→ OP_SET_GLOBAL 1  Stack: [], globals[1] = 8
→ OP_GET_GLOBAL 2  Stack: [<função print>]
→ OP_GET_GLOBAL 1  Stack: [<função print>, 8]
→ OP_CALL 1        Imprime: 8
```

---

## 🔧 Debugging

### Dumper de Bytecode (`libbu/src/debug.cpp`)
```bash
./natives script.bu --dump
```

### Logs do Runtime
```cpp
Log("[VM] Creating array...");
```

### Ferramentas:
- `printTokens()` - Lexer
- `dumpFunction()` - Bytecode
- `printValue()` - Runtime values

---

## 🚀 Performance

### Otimizações implementadas:
- ✅ Goto dispatch (vs switch)
- ✅ String interning
- ✅ Arena allocator
- ✅ Inline cache para método lookup
- ✅ Direct-threaded code
- ✅ Local variables em stack (não heap)

### Benchmarks típicos:
- Fibonacci recursivo: ~15M calls/sec
- Array operations: ~50M ops/sec
- String concat: ~5M ops/sec

---

## 📚 Referências Internas

- `NATIVE_BINDINGS.md` - Como criar bindings
- `OPTIMIZATIONS.md` - Detalhes de otimização
- `TRY_CATCH_LIMITATIONS.md` - Limitações de exceções
- `BUILTIN_METHODS.md` - Métodos built-in

---

## 🎓 Resumo Para Não Te Perderes

1. **Lexer** → Transforma texto em tokens
2. **Compiler** → Transforma tokens em bytecode
3. **Interpreter** → Executa bytecode com stack VM
4. **GC** → Limpa memória automaticamente
5. **Processes/Fibers** → Multi-tasking cooperativo
6. **Bindings** → Liga C++ (Raylib) ao BuLang

**Regra de ouro:** Tudo é Value, tudo vai para a stack, o GC cuida do resto! 🚀
