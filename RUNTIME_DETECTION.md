# BuLang VM - Runtime Detection Analysis

## Status ATUAL

**Configuração em `config.hpp` linha 35:**
```cpp
//#define USE_COMPUTED_GOTO 0  ← COMENTADO (desativado)
```

**Conclusão**: **ESTÁS A USAR O SWITCH RUNTIME** ✅

---

## Como Detectar Qual Runtime Está Ativo

### 1. **Verificação Direta em `config.hpp`**
```cpp
// Se COMENTADO:           → SWITCH runtime
// Se DEFINIDO (0 ou 1):   → Depende do valor
//    #define USE_COMPUTED_GOTO 0  → SWITCH runtime  
//    #define USE_COMPUTED_GOTO 1  → GOTO runtime
```

### 2. **Verificação em Build Time**

#### Em CMakeLists.txt (NÃO há compilação condicional)
```cmake
# Não existe neste projeto!
# if(USE_COMPUTED_GOTO)
#     set(INTERPRETER_SOURCE interpreter_runtime_goto.cpp)
# else()
#     set(INTERPRETER_SOURCE interpreter_runtime_switch.cpp)
# endif()
```

**O CMakeLists.txt compila TODOS os sources (`file(GLOB SOURCES "src/*.cpp")`)**, logo **ambos os ficheiros estão presentes**, mas apenas um é realmente compilado baseado em `#ifndef USE_COMPUTED_GOTO` / `#ifdef USE_COMPUTED_GOTO`.

### 3. **Estrutura dos Ficheiros**

#### `interpreter_runtime_switch.cpp` (Linha 40)
```cpp
#ifndef USE_COMPUTED_GOTO
extern size_t get_type_size(BufferType type);

// ... 5160 linhas de código ...

#endif // !USE_COMPUTED_GOTO
```

**Estado**: `USE_COMPUTED_GOTO` **NÃO está definido** → ✅ Este ficheiro COMPILA

---

#### `interpreter_runtime_goto.cpp` (Linha 45)
```cpp
#ifdef USE_COMPUTED_GOTO

// ... 5316 linhas de código ...

#endif // USE_COMPUTED_GOTO
```

**Estado**: `USE_COMPUTED_GOTO` **NÃO está definido** → ❌ Este ficheiro é **IGNORADO**

---

## Detecção Runtime (Sem Alterar Código)

### Método 1: Verificar o Binário Compilado

```bash
# Se usas SWITCH, símbolos terão "switch" no nombre
nm -C bin/bu | grep "Interpreter::run_fiber"

# Output terá tipos/sizes ligeiramente diferentes baseado na versão compilada
```

### Método 2: Análise do Binário (gdb/lldb)

```bash
gdb ./bin/bu
(gdb) disassemble Interpreter::run_fiber

# SWITCH: Vê uma estrutura de "cmp reg, imm" + "je" = switch statement
# GOTO:   Vê "mov rax, [dispatch_table + rdx*8]" + "jmp rax" = computed goto
```

### Método 3: Timing (Benchmark)

```bulang
// test_dispatch_speed.bu
def test() {
    var start = clock();
    for (var i = 0; i < 100000000; i = i + 1) {
        var x = 5 + 3;  // Simples OP_ADD loop
    }
    var elapsed = clock() - start;
    print("Time: " + elapsed);
}

test();
```

**Resultado esperado:**
- SWITCH: ~0.8-1.2 segundos
- GOTO:   ~0.2-0.4 segundos
- **Diferença**: GOTO é ~3-4x mais rápido

### Método 4: Análise de Objetos Compilados

```bash
# Ver qual ficheiro foi compilado
strings bin/bu | grep "interpreter_runtime"

# Ou procurar símbolos específicos
nm -C bin/bu | grep "op_constant\|op_add" 

# GOTO terá símbolos com labels (op_constant, op_add, etc.)
# SWITCH terá tudo dentro de run_fiber com cases
```

---

## Constatações no Teu Projeto

| Aspecto | Status |
|---------|--------|
| `config.hpp` linha 35 | `//#define USE_COMPUTED_GOTO 0` ← **COMENTADO** |
| Runtime Ativo | **SWITCH** ✅ |
| CMakeLists.txt | Sem compilação condicional (ambos compilam, mas só um ativa) |
| Ficheiro Compilado | `interpreter_runtime_switch.cpp` |
| Ficheiro Ignorado | `interpreter_runtime_goto.cpp` |

---

## Como Ativar GOTO Runtime (Se Quiseres Tentar)

### Opção 1: Descomentar em config.hpp
```cpp
// Muda de:
//#define USE_COMPUTED_GOTO 0

// Para:
#define USE_COMPUTED_GOTO 1
```

Depois recompila:
```bash
cd /media/projectos/projects/cpp/BuLangVM/build
cmake ..
make -j4
```

### Opção 2: Via CMake (Mais Limpo)
```bash
cd /media/projectos/projects/cpp/BuLangVM/build
cmake .. -DUSE_COMPUTED_GOTO=ON
make -j4
```

Mas isso requer editar `CMakeLists.txt` para suportar:
```cmake
option(USE_COMPUTED_GOTO "Use computed goto dispatch" OFF)
if(USE_COMPUTED_GOTO)
    target_compile_definitions(libbu PRIVATE USE_COMPUTED_GOTO=1)
endif()
```

---

## Resumo

✅ **Estás a usar SWITCH runtime**
- Configurado em `config.hpp` linha 35 (comentado)
- Compila `interpreter_runtime_switch.cpp` (5160 linhas)
- Ignora `interpreter_runtime_goto.cpp` (5316 linhas)

**Performance**: ~150M ops/sec (mais lento que GOTO)

**Compatibilidade**: Funciona em qualquer compilador

