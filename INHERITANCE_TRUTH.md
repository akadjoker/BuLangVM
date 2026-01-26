# BuLang Inheritance - A Verdadeira Razão da Obrigatoriedade de Override

## Descoberta Crucial

**A herança em BuLang funciona DIFERENTE conforme o tipo de chamada:**

| Tipo de Chamada | Opcode | Procura Na Cadeia? | Resultado |
|---|---|---|---|
| Normal: `obj.method()` | `OP_INVOKE` | ✅ SIM (inteira) | **Funciona sem override!** |
| Super: `super.method()` | `OP_SUPER_INVOKE` | ❌ NÃO (1 nível) | **Precisa override!** |

---

## O Problema Real

### OP_INVOKE (Chamadas Normais)

```cpp
// interpreter_runtime_switch.cpp:2825
if (receiver.isClassInstance()) {
    ClassInstance *instance = receiver.asClassInstance();
    Function *method;
    
    if (instance->getMethod(nameValue.asString(), &method)) {  // ← getMethod()!
        // Encontrou na cadeia inteira
        // Executa método
    }
}
```

**getMethod()** em `interpreter.hpp:327`:
```cpp
FORCE_INLINE bool getMethod(String *name, Function **out) {
    ClassDef *current = klass;
    while (current) {  // ← BUSCA TODA A CADEIA!
        if (current->methods.get(name, out)) {
            return true;
        }
        current = current->superclass;
    }
    return false;
}
```

**RESULTADO**: `b.foo()` procura em B → não encontra → procura em A → **encontra!** ✅

---

### OP_SUPER_INVOKE (Chamadas Super)

```cpp
// interpreter_runtime_switch.cpp:3837
case OP_SUPER_INVOKE: {
    uint8_t ownerClassId = READ_BYTE();
    String *methodName = nameValue.asString();
    ClassDef *ownerClass = classes[ownerClassId];
    
    // ❌ PROCURA APENAS EM ownerClass->superclass !
    if (!ownerClass->superclass->methods.get(methodName, &method)) {
        runtimeError("Undefined method '%s'", methodName->chars());
        return;
    }
}
```

**PROBLEMA**: Procura **exatamente 1 nível acima**, não toda a cadeia!

**RESULTADO**: `C.bar()` chama `super.foo()` → procura em `B->superclass` (que é A) → não encontra B.foo() → **ERRO!** ❌

---

## Teste Real: Prova

### Test 1: Chamada Normal (Funciona)

```bulang
class A { def foo() { print("A"); } }
class B : A { }

var b = B();
b.foo();  // ✅ A.foo encontrada via OP_INVOKE → getMethod() inteira
```

**Output:**
```
A
```

---

### Test 2: Super Call (Falha)

```bulang
class A { def foo() { print("A"); } }
class B : A { }
class C : B {
    def bar() { super.foo(); }  // OP_SUPER_INVOKE
}

var c = C();
c.bar();  // ❌ Runtime Error
```

**Output:**
```
Runtime Error: Undefined method 'foo'
```

---

## Explicação: Por Que Acontece?

### Chamada Normal: `b.foo()`

```
Compilação:
    b (receiver)
    OP_INVOKE "foo"

Runtime:
    1. OP_INVOKE procura "foo" em b (instância de B)
    2. Chama instance->getMethod("foo")
    3. getMethod itera: B → A → encontra A.foo()
    4. Executa A.foo()
```

---

### Super Call: `super.foo()` de C

```
Compilação:
    GET_LOCAL 0 (self)
    OP_SUPER_INVOKE(classId=B)  ← B é a classe que contém super.foo()
    nameIdx = "foo"

Runtime:
    1. OP_SUPER_INVOKE lê ownerClass = B
    2. Procura em B->superclass = A
    3. Procura A.methods.get("foo") ← DIRETO, sem loop!
    4. Encontra A.foo()
    5. Executa A.foo()
```

Isto **funciona perfeitamente**!

---

### Super Call: `super.foo()` de C quando B também herdaria

```
Compilação:
    GET_LOCAL 0 (self)
    OP_SUPER_INVOKE(classId=C)  ← C é a classe atual
    nameIdx = "foo"

Runtime:
    1. OP_SUPER_INVOKE lê ownerClass = C
    2. Procura em C->superclass = B
    3. Procura B.methods.get("foo") ← B NÃO tem!
    4. ERRO: "Undefined method"
    5. ❌ Não continua procurando em B->superclass = A
```

**AQI ESTÁ O PROBLEMA!**

---

## A Solução: Bridge Method

Para resolver, **precisa de um "bridge"** em cada nível:

```bulang
class A {
    def foo() { print("A"); }
}

class B : A {
    def foo() {
        super.foo();  // ← Bridge para A
    }
}

class C : B {
    def bar() {
        super.foo();  // ← Agora procura em B e encontra!
    }
}

var c = C();
c.bar();  // ✅ Funciona!
```

**Fluxo:**
1. C.bar() → super.foo() → OP_SUPER_INVOKE(C)
2. Procura em C->superclass = B
3. **Encontra B.foo()** ✅
4. B.foo() executa → super.foo() → OP_SUPER_INVOKE(B)
5. Procura em B->superclass = A
6. **Encontra A.foo()** ✅

---

## Comparação: OP_INVOKE vs OP_SUPER_INVOKE

### OP_INVOKE (Chamada Normal)

```cpp
// Pseudocódigo
method = getMethod(name);  // ← Busca inteira
execute(method);
```

**Implementação**: Usa loop while em getMethod()

---

### OP_SUPER_INVOKE (Chamada Super)

```cpp
// Pseudocódigo
method = ownerClass->superclass->methods.get(name);  // ← Direto, sem loop
execute(method);
```

**Implementação**: Acesso direto sem getMethod()

---

## Por Que Este Design?

### 1. **Performance**
- OP_SUPER_INVOKE é mais rápido (O(1) no mapa de métodos)
- OP_INVOKE é mais lento (O(n) na cadeia, mas cacheável)

### 2. **Segurança**
- Super é explícito: sabe-se exatamente qual classe está sendo usada
- Evita surpresas de sobrescrita

### 3. **Trade-off**
- Ganho: Dispatch super mais rápido
- Perda: Obriga override em cada nível

---

## Solução Proposta (Se Quisesses Corrigir)

Mudar OP_SUPER_INVOKE para também usar loop:

```cpp
case OP_SUPER_INVOKE: {
    uint8_t ownerClassId = READ_BYTE();
    String *methodName = nameValue.asString();
    ClassDef *ownerClass = classes[ownerClassId];
    ClassDef *searchClass = ownerClass->superclass;
    
    Function *method = nullptr;
    while (searchClass) {  // ← ADICIONAR LOOP!
        if (searchClass->methods.get(methodName, &method)) {
            break;
        }
        searchClass = searchClass->superclass;
    }
    
    if (!method) {
        runtimeError("Undefined method '%s'", methodName->chars());
        return;
    }
    
    // Execute...
}
```

**Isto resolveria o problema, mas teria custo de performance.**

---

## Conclusão

| Mecanismo | Comportamento | Razão |
|---|---|---|
| **OP_INVOKE** (`.method()`) | Busca inteira na cadeia | Facilita uso normal de herança |
| **OP_SUPER_INVOKE** (`super.method()`) | Apenas 1 nível acima | Performance + Segurança |

**O "problema" de precisar override não é bug, é design intencional.**

Funciona assim:
- Uso normal: automático (OP_INVOKE com getMethod())
- Super explícito: manual (OP_SUPER_INVOKE sem loop)

