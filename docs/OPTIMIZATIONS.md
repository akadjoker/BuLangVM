# BuLang VM - Plano de Otimizações

## Benchmark Atual (Release Mode) vs Python

| Teste | BuLang u16 | Python | Ratio | Nota |
|-------|------------|--------|-------|------|
| Simple increment loop (10k) | 1257.70 μs | 231.15 μs | **5.4x** | Overhead de dispatch |
| Multiple global variables | 1294.60 μs | 87.33 μs | **14.8x** | Hash lookup globals |
| Local variables in function | 718.60 μs | 40.61 μs | **17.7x** | Stack overhead |
| Prefix increment | 1110.40 μs | - | - | Múltiplas instruções |
| Postfix increment | 1200.70 μs | - | - | Múltiplas instruções |
| Arithmetic operations | 1317.10 μs | 350.32 μs | **3.8x** | ✅ Razoável |
| Many variables (50) | 5035.70 μs | 6.66 μs | **756x** 🔴 | Hash lookup globals |
| Recursive Fibonacci (20) | 2666.70 μs | 882.57 μs | **3.0x** | ✅ Bom! |
| Recursive Factorial (15) | 720.40 μs | 0.85 μs | **847x** 🔴 | Call overhead |
| **Array manipulation (1000)** | 709.50 μs | 104.00 μs | **6.8x** | ✅ Aceitável |
| **Object properties (1000)** | 1233.10 μs | 115.52 μs | **10.7x** | Hash lookup props |
| **Physics simulation (100)** | 2445.90 μs | 1376.64 μs | **1.8x** | 🎉 Excelente! |
| **Nested loops (100x100)** | 1643.70 μs | 432.48 μs | **3.8x** | ✅ Bom |

### Análise

**Pontos Fortes:**
- ✅ Physics simulation: 1.8x (uso real!)
- ✅ Fibonacci recursivo: 3.0x
- ✅ Nested loops: 3.8x

**Pontos Fracos:**
- 🔴 Hash lookup de globais: 15-756x mais lento
- 🔴 Múltiplas instruções para incremento: 5-15x
- 🔴 Hash lookup de propriedades: 10x

---

## Otimizações Prioritárias (Baseadas nos Benchmarks)

### 🔥 1. Globais por Índice Direto (MÁXIMA PRIORIDADE)

**Problema Crítico**: `many variables (50)` = **756x mais lento** devido a hash lookup!

**Atual**:
```cpp
// Runtime para OP_GET_GLOBAL
String* name = readString();
Value v = globals_.get(name);  // HashMap lookup O(log n)
```

**Solução**:
```cpp
// Compilador mantém índice
HashMap<String*, uint16> globalIndices_;
Vector<Value> globalsArray_;  // Runtime

// Compilação
uint16 idx = getGlobalIndex(name);  // ou cria novo
emitVarOp(OP_GET_GLOBAL, idx);

// Runtime
uint16 idx = READ_U16();
Value v = globalsArray_[idx];  // O(1) array access!
```

**Ganho esperado**: **50-100x** em código com muitas globais, **~30%** geral.

---

### 🔥 2. Superinstruções para Incremento (ALTA PRIORIDADE)

**Problema**: `i++` gera 5 instruções + dispatch overhead.

**Atual** (para `i++`):
```
OP_GET_LOCAL 0      // 1. load i
OP_CONSTANT 1       // 2. load 1  
OP_ADD              // 3. add
OP_SET_LOCAL 0      // 4. store i
OP_POP              // 5. discard
```

**Solução**: Uma instrução faz tudo.
```cpp
// Novos opcodes
OP_INC_LOCAL   slot     // i++ local
OP_DEC_LOCAL   slot     // i-- local
OP_INC_GLOBAL  idx      // global++
OP_DEC_GLOBAL  idx      // global--

// Runtime (exemplo)
case OP_INC_LOCAL: {
    uint8_t slot = READ_BYTE();
    stack_[framePtr + slot].number += 1.0;  // Direto!
    break;
}
```

**Ganho esperado**: **3-5x** em loops simples, **~20%** geral.

---

### 🔥 3. Inline Caching para Propriedades (ALTA PRIORIDADE)

**Problema**: `self.x` faz hash lookup cada acesso.

**Atual**:
```cpp
// self.x = self.x + 1
OP_GET_LOCAL self
OP_GET_PROPERTY "x"   // hash("x") → lookup
OP_CONSTANT 1
OP_ADD
OP_GET_LOCAL self
OP_SET_PROPERTY "x"   // hash("x") → lookup
```

**Solução**: Cache do offset após primeiro lookup.

```cpp
struct InlineCache {
    Shape* cachedShape;
    uint16 cachedOffset;
};

case OP_GET_PROPERTY_CACHED: {
    InlineCache* cache = &caches[READ_U16()];
    Object* obj = AS_OBJECT(peek(0));

    if (obj->shape == cache->cachedShape) {
        // Fast path: acesso direto
        push(obj->fields[cache->cachedOffset]);
    } else {
        // Slow path: lookup + update cache
        // ...
    }
}
```

**Ganho esperado**: **30-50%** em código OOP intensivo (object properties).

---

### 4. Fast Path para Loops Comuns (MÉDIA PRIORIDADE)

**Problema**: While loop tem muito overhead.

**Atual** (para `while (i < 1000) { ... }`):
```
LOOP:
  OP_GET_LOCAL i      // load
  OP_CONSTANT 1000    // load limit
  OP_LESS             // compare
  OP_JUMP_IF_FALSE    // branch
  ... body ...
  OP_LOOP LOOP        // jump back
```

**Solução**: Detectar padrão comum e criar superinstrução.
```cpp
// Detectar: while (i < N) { body; i++; }
OP_LOOP_COUNTED  slot=i, limit=1000, body_size=X
```

**Ganho esperado**: **10-20%** em loops simples.

---

### 5. Otimizar Constant Pool Access (BAIXA PRIORIDADE)

**Problema**: Constantes grandes (números, strings) carregadas repetidamente.

**Solução**: Cache de constantes usadas frequentemente.

**Ganho esperado**: **5-10%** em código com muitas constantes.

---

## Prioridade de Implementação (Atualizada)

| # | Otimização | Esforço | Impacto Real | Prioridade | Ganho Esperado |
|---|------------|---------|--------------|------------|----------------|
| 1 | **Globais por índice** | Médio | **Enorme** (756x→10x) | 🔥 **CRÍTICA** | **50-100x** |
| 2 | **OP_INC/DEC** | Baixo | Grande | 🔥 **Alta** | **3-5x loops** |
| 3 | **Inline caching props** | Alto | Grande | 🔥 **Alta** | **3-5x OOP** |
| 4 | Loop superinstruções | Médio | Médio | Média | **10-20%** |
| 5 | Constant pool cache | Baixo | Baixo | Baixa | **5-10%** |
| 6 | NaN-boxing | Muito Alto | Alto | Baixa | **20-40%** |

---

## Roadmap de Implementação

### ✅ Fase 0: Baseline Estabelecida
- [x] Benchmarks completos (sintéticos + realistas)
- [x] Comparação com Python
- [x] Identificação de bottlenecks

### 🚀 Fase 1: Quick Wins (2-3 dias)

**1.1 Globais por Índice** (1 dia)
```cpp
// compiler.hpp
HashMap<String*, uint16> globalIndices_;
uint16 nextGlobalIndex_ = 0;

uint16 getOrCreateGlobalIndex(String* name) {
    auto it = globalIndices_.find(name);
    if (it != globalIndices_.end()) return it->second;
    uint16 idx = nextGlobalIndex_++;
    globalIndices_[name] = idx;
    return idx;
}

// Emissão
void namedVariable(Token name, bool canAssign) {
    uint16 idx = getOrCreateGlobalIndex(name.lexeme);
    if (canAssign && match(TOKEN_EQUAL)) {
        expression();
        emitVarOp(OP_SET_GLOBAL, idx);
    } else {
        emitVarOp(OP_GET_GLOBAL, idx);
    }
}
```

```cpp
// interpreter.hpp
Vector<Value> globalsArray_;  // Novo!

// Runtime
case OP_GET_GLOBAL: {
    uint16 idx = READ_U16();
    if (idx >= globalsArray_.size()) {
        runtimeError("Undefined global");
        return false;
    }
    push(globalsArray_[idx]);
    break;
}
```

**1.2 Superinstruções de Incremento** (1-2 dias)
```cpp
// opcode.hpp
OP_INC_LOCAL,   // slot (u8)
OP_DEC_LOCAL,   // slot (u8)  
OP_INC_GLOBAL,  // index (u16)
OP_DEC_GLOBAL,  // index (u16)

// compiler_statements.cpp - Detectar padrão
void prefixIncrement() {
    consume(TOKEN_PLUS_PLUS);
    
    if (match(TOKEN_IDENTIFIER)) {
        Token name = previous();
        
        // Resolver variável
        int local = resolveLocal(&name);
        if (local != -1) {
            emitBytes(OP_INC_LOCAL, (uint8_t)local);  // Uma instrução!
            return;
        }
        
        uint16 global = getOrCreateGlobalIndex(name.lexeme);
        emitVarOp(OP_INC_GLOBAL, global);  // Uma instrução!
    }
}
```

**Teste após Fase 1**: Esperar **many variables** cair de 756x → ~10x

### 🔧 Fase 2: Otimizações Médias (1 semana)

**2.1 Inline Caching para Propriedades**
- Adicionar `InlineCache` array no código compilado
- `OP_GET_PROPERTY_CACHED` com fallback
- Medir impacto em physics simulation

**2.2 Loop Counting**
- Detectar loops `while (i < N)` padrão
- Emitir `OP_LOOP_COUNTED`

### 🎯 Fase 3: Otimizações Avançadas (2+ semanas)

**3.1 NaN-Boxing**
- Reescrever `Value` para 64 bits
- Migrar todo código
- **Grande refactor**, só se necessário

---

## Métricas de Sucesso

Após otimizações da Fase 1, objetivos realistas:

| Teste | Atual | Fase 1 | Fase 2 | Fase 3 |
|-------|-------|--------|--------|--------|
| Many variables (50) | **756x** 🔴 | **~10x** ✅ | ~5x | ~3x |
| Simple increment | 5.4x | **~2x** ✅ | ~1.5x | ~1.2x |
| Object properties | 10.7x | 10x | **~3x** ✅ | ~2x |
| Physics simulation | 1.8x | **~1.2x** ✅ | ~1.1x | **~0.9x** 🎉 |
| **Média geral** | **~10x** | **~3x** ✅ | **~2x** | **~1.5x** |

**Meta Final**: BuLang **mais rápido** que Python em physics/games! 🚀

---

## Notas Importantes

### Por que BuLang já é bom?

1. **Em uso real (raylib)**: Já bate Python! O overhead está nas chamadas nativas (C++)
2. **Physics simulation**: Apenas 1.8x mais lento - muito competitivo
3. **Recursão**: 3x é excelente para VM escrita à mão

### O que realmente importa?

- ❌ **Não importa**: Loops vazios (ninguém faz isso)
- ✅ **Importa**: Arrays, objetos, física, loops com trabalho real
- 🎯 **Crítico**: Acesso a globais (756x é inaceitável)

### Estratégia

1. **Fase 1 primeiro**: Globais + incremento = 80% do ganho
2. **Medir tudo**: Benchmark após cada otimização
3. **Não complicar**: Manter código simples e legível
4. **NaN-boxing**: Só se realmente necessário (grande refactor)

---

## Próximos Passos

1. [ ] Implementar globais por índice
2. [ ] Implementar OP_INC/DEC  
3. [ ] Rodar benchmarks
4. [ ] Medir ganho real
5. [ ] Decidir se Fase 2 é necessária

**Começar por**: Globais por índice (maior impacto, esforço médio)

