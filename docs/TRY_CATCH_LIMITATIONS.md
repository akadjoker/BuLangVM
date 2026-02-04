# Try/Catch - Limitações e Plano Futuro

## Estado Actual

### ✅ O que FUNCIONA:

1. **Try/catch básico** - Captura erros e executa catch
2. **Finally** - Executa sempre, mesmo com return
3. **Throw manual** - `throw "erro"` funciona
4. **Try/catch nested** - Múltiplos níveis funcionam
5. **Try/catch em funções** - Funciona com return
6. **Try/catch em loops** - Múltiplas iterações
7. **Divisão por zero** - Via macro BINARY_OP_THROW
8. **Type errors** - string * number, etc.
9. **Operações com nil** - nil + 10, etc.
10. **Return dentro de try** - Finally executa antes do return

### ❌ O que NÃO FUNCIONA (não é capturado):

1. **Array index out of bounds** - `arr[100]`
2. **Undefined global variable** - Acesso a variável não definida
3. **Stack overflow** - Recursão infinita
4. **Invalid function call** - Chamar algo que não é função
5. **Wrong argument count** - Argumentos a mais/menos
6. **Field access on nil** - `nil.field`
7. **Native class errors** - Erros em classes nativas
8. **Module errors** - Erros em módulos externos
9. **Process/Fiber errors** - Alguns erros internos
10. **Closure errors** - Alguns erros de closures

### 📊 Estatísticas:
- **239** locais com `runtimeError()` no runtime switch
- A maioria usa `runtimeError()` que termina execução
- Apenas alguns usam `throwException()` ou macros THROW

---

## Tipos de Erro

### Categoria A: Erros Recuperáveis (DEVEM ser capturados)
Erros que o programador pode querer tratar:
- Array out of bounds
- Map key not found
- Type mismatches
- Division by zero ✅ (já funciona)
- Nil operations ✅ (já funciona)

### Categoria B: Erros de Programação (podem ser capturados)
Erros que indicam bugs no código:
- Wrong argument count
- Field access on nil
- Undefined variables (runtime)

### Categoria C: Erros Fatais (NÃO devem ser capturados)
Erros que indicam estado corrompido:
- Stack overflow
- Internal VM errors
- Memory allocation failures
- Invalid bytecode

---

## Plano de Implementação

### Fase 1: Criar macro/função helper (FÁCIL)
```cpp
// Em vez de:
runtimeError("Array index %d out of bounds", i);
return FIBER_DONE;

// Usar:
THROW_ERROR("Array index %d out of bounds", i);
// Que internamente faz:
// 1. Cria Value com mensagem
// 2. Chama throwException()
// 3. Se não há handler, faz runtimeError() original
```

### Fase 2: Converter erros Categoria A (PRIORITÁRIO)
1. Array index out of bounds ⭐
2. Map key not found (se aplicável)
3. String index out of bounds
4. Buffer bounds checks

### Fase 3: Converter erros Categoria B (OPCIONAL)
1. Wrong argument count
2. Field access on nil/wrong type
3. Method not found

### Fase 4: Manter Categoria C como fatal
- Stack overflow → fatal
- Invalid bytecode → fatal
- Internal errors → fatal

---

## Implementação Proposta

### 1. Macro THROW_RECOVERABLE
```cpp
#define THROW_RECOVERABLE(fmt, ...)                              \
    do {                                                         \
        char _buf[256];                                          \
        snprintf(_buf, sizeof(_buf), fmt, ##__VA_ARGS__);        \
        Value _err = makeString(createString(_buf));             \
        if (!throwException(_err)) {                             \
            runtimeError(fmt, ##__VA_ARGS__);                    \
            return {FiberResult::ERROR, instructionsRun, 0, 0};  \
        }                                                        \
    } while(0)
```

### 2. Exemplo de conversão
```cpp
// ANTES:
if (i < 0 || i >= size) {
    runtimeError("Array index %d out of bounds (size=%d)", i, size);
    return {FiberResult::ERROR, ...};
}

// DEPOIS:
if (i < 0 || i >= size) {
    THROW_RECOVERABLE("Array index %d out of bounds (size=%d)", i, size);
}
```

---

## Estimativa de Esforço

| Tarefa | Dificuldade | Impacto | Prioridade |
|--------|-------------|---------|------------|
| Criar macro THROW_RECOVERABLE | Fácil | Alto | ⭐⭐⭐ |
| Array bounds | Médio | Alto | ⭐⭐⭐ |
| Map key errors | Fácil | Médio | ⭐⭐ |
| Field access | Médio | Médio | ⭐⭐ |
| Argument count | Difícil | Baixo | ⭐ |

---

## Uso Recomendado (Documentação)

### ✅ Pode usar try/catch para:
```bulang
try {
    var x = 10 / n;  // Divisão por zero
    var s = "a" + b; // Se b não é string
} catch (e) {
    print("Erro: " + e);
}
```

### ⚠️ NÃO usar try/catch para (ainda):
```bulang
// ESTES ERROS NÃO SÃO CAPTURADOS!
try {
    var arr = [1, 2, 3];
    var x = arr[100];  // ❌ Fatal
} catch (e) {
    // Nunca chega aqui
}
```

### Workaround actual:
```bulang
// Verificar manualmente antes:
var arr = [1, 2, 3];
if (i >= 0 && i < len(arr)) {
    var x = arr[i];  // Seguro
} else {
    print("Índice inválido!");
}
```

---

## Conclusão

O try/catch actual é **funcional para erros aritméticos e de tipo**, mas **limitado para erros de acesso** (arrays, maps, fields).

**Recomendação:** Implementar Fase 1 e 2 para ter um sistema de exceções mais completo e útil.
