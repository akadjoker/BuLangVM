# BuLang Inheritance Analysis - Por Que Override Obrigatório?

## TL;DR

BuLang **NÃO faz busca automática na cadeia de herança** durante dispatch. Quando chamas um método, BuLang procura **APENAS na classe atual**, não no parent.

---

## 1. Como Funciona a Procura de Métodos

### Código em `interpreter.hpp` (Linha 327)

```cpp
// ClassInstance::getMethod() - INLINE FAST PATH
FORCE_INLINE bool getMethod(String *name, Function **out)
{
    ClassDef *current = klass;
    while (current) {
        if (current->methods.get(name, out)) {
            return true;  // ← Encontrou!
        }
        current = current->superclass;  // ← Sobe na cadeia
    }
    return false;  // ← Não encontrou em nenhum nível
}
```

**WAIT!** Isto **SIM** faz busca na cadeia inteira! 

Então o que é o problema? Vamos ver onde `getMethod` é **realmente usado**...

---

## 2. Quando getMethod É Usado

### OP_CALL (Chamada de Método Normal)

Quando chamas `sprite.update(elapsed)`:

```cpp
// interpreter_runtime_switch.cpp:799 - OP_CALL
case OP_CALL: {
    uint8 argCount = READ_BYTE();
    Value callee = NPEEK(argCount);  // ← A função/método
    
    if (callee.isFunction()) {
        // Executa a função
        // getMethod() NÃO é chamado aqui!
    }
    // ...
}
```

**O método já tem que estar no stack antes!** Como chegou lá?

---

## 3. Como o Método Chega ao Stack?

### OP_GET_PROPERTY (Acesso a Propriedade/Método)

Quando compila `sprite.update`:

```cpp
// compiler_statements.cpp - compilation
emitBytes(OP_GET_PROPERTY, nameIdx);  // Push método na stack
```

Depois em runtime:

```cpp
// interpreter_runtime_switch.cpp:1525
case OP_GET_PROPERTY: {
    Value object = PEEK();
    Value nameValue = READ_CONSTANT();
    
    if (object.isClassInstance()) {
        ClassInstance *instance = object.asClassInstance();
        
        uint8_t fieldIdx;
        if (instance->klass->fieldNames.get(nameValue.asString(), &fieldIdx)) {
            // Campo encontrado na CLASSE ATUAL
            PUSH(instance->fields[fieldIdx]);
            break;
        }
        
        // NativeProperty... mas métodos?
        // ❌ NÃO HÁ BUSCA DE MÉTODOS AQUI!
        
        runtimeError("Undefined property '%s'", name);
        PUSH(makeNil());
        return;
    }
}
```

**PROBLEMA IDENTIFICADO**: `OP_GET_PROPERTY` não procura métodos! Apenas propriedades/campos!

---

## 4. Então Como Métodos São Encontrados?

### Durante COMPILAÇÃO, Não Runtime!

Quando o compilador vê `sprite.update`, ele **não sabe se é um método**. Então gera:

```
1. OP_GET_PROPERTY (tenta propriedade → falha)
2. Se falhar? Erro de compilação!
```

**Solução**: Quando declaras `Sprite : Object` e compila `super.update()`, o compilador **VERIFICA EM COMPILE TIME**:

```cpp
// compiler_statements.cpp:2668 - classDeclaration()
if (currentClass->superclass == nullptr) {
    error("Cannot use 'super' in a class with no superclass");
    return;
}

// Compila: OP_SUPER_INVOKE (opcode especial)
emitBytes(OP_SUPER_INVOKE, currentClass->index);
```

---

## 5. O Sistema de SUPER vs Métodos Normais

### Chamada Normal: `sprite.update(elapsed)`

```
Compilação:
    1. Gera OP_GET_PROPERTY "update"
    2. Gera OP_CALL
    
Runtime:
    1. OP_GET_PROPERTY procura "update" em Sprite.methods
    2. Se não encontrar → ERRO
    3. Se encontrar → OP_CALL executa
```

**Resultado**: Sprite PRECISA definir `update()` explicitamente!

---

### Chamada via Super: `super.update(elapsed)`

```
Compilação:
    1. Gera OP_GET_LOCAL 0  (self)
    2. Gera OP_SUPER_INVOKE (ownerClassId=Sprite)
    
Runtime:
    1. OP_SUPER_INVOKE lê ownerClass (Sprite)
    2. Procura método em ownerClass->superclass (Object)
    3. Encontra Object.update()
    4. Executa na stack atual
```

**Resultado**: Super funciona! Mas précisa chamar explicitamente `super.update()`

---

## 6. O Problema: Herança Multinível

### Cadeia: `Particle extends Sprite extends Object`

```
Object.update() {
    // Atualiza posição
}

Sprite.update() {
    // NÃO DEFINIDO! ← PROBLEMA
}

Particle.update() {
    super.update(elapsed);  // ← Procura em Sprite!
}
```

**O que acontece:**

1. Compilador vê `Particle extends Sprite`
2. `super.update()` gera `OP_SUPER_INVOKE(Sprite)`
3. Runtime procura em `Sprite.methods.get("update")`
4. **NÃO ENCONTRA** (Sprite não override)
5. **ERRO**: "Undefined method 'update'"

---

## 7. A Solução: Override em Cada Nível

```cpp
Object {
    def update(elapsed) {
        updateMotion(elapsed);
    }
}

Sprite : Object {
    def update(elapsed) {
        super.update(elapsed);  // ← Cadeia ligada!
    }
}

Particle : Sprite {
    def update(elapsed) {
        super.update(elapsed);  // ← Agora encontra Sprite.update()
        self.life -= elapsed;
    }
}
```

**Por que funciona:**
- Particle.update() → super → **encontra Sprite.update()** ✅
- Sprite.update() → super → **encontra Object.update()** ✅
- Object.update() → sem super (fim da cadeia) ✅

---

## 8. Comparação com Outras Linguagens

| Linguagem | Mecanismo | Resultado |
|-----------|-----------|-----------|
| **Java/C#** | Virtual method dispatch | Automático (todos chamam correto) |
| **Python** | Method Resolution Order (MRO) | Automático com `super()` |
| **C++** | Virtual function pointers | Automático se virtual declarado |
| **BuLang** | Direct method table lookup | **MANUAL - precisa override** |

---

## 9. Código Exato: OP_SUPER_INVOKE

```cpp
// interpreter_runtime_switch.cpp:3837
case OP_SUPER_INVOKE: {
    uint8_t ownerClassId = READ_BYTE();  // A classe que fez o super.call
    uint8_t nameIdx = READ_BYTE();       // Nome do método
    uint8_t argCount = READ_BYTE();
    
    String *methodName = func->chunk->constants[nameIdx];
    Value self = NPEEK(argCount);
    ClassInstance *instance = self.asClassInstance();
    ClassDef *ownerClass = classes[ownerClassId];  // ← Sprite
    
    // KEY: Procura APENAS na superclass do ownerClass
    if (!ownerClass->superclass->methods.get(methodName, &method)) {
        runtimeError("Undefined method '%s'", methodName->chars());
        return;
    }
    
    // Execute a função encontrada
}
```

**O PROBLEMA AQUI**: Procura **exatamente um nível acima** (ownerClass->superclass), não toda a cadeia!

---

## 10. Por Que Este Design?

### Vantagens:
1. **Explícito**: Sabe-se exatamente qual método é chamado
2. **Sem surpresas**: Sem "magic" de MRO complexa
3. **Fast**: Dispatch é O(1) - sem busca em tempo de runtime
4. **Seguro**: Evita ambiguidades em herança múltipla

### Desvantagens:
1. **Verboso**: Precisa override em cada nível
2. **Frágil**: Quebra se esquecer um nível na cadeia
3. **Manual**: Não é automático como linguagens OOP modernas

---

## 11. Teste: Confirmação do Comportamento

```bulang
class A {
    def foo() { print("A.foo"); }
}

class B : A {
    // NÃO override foo()
}

class C : B {
    def bar() {
        super.foo();  // Procura em B, não em A!
    }
}

var c = C();
c.bar();  // ❌ ERRO: "Undefined method 'foo'"
```

**Fix**:
```bulang
class B : A {
    def foo() {
        super.foo();  // ← Adiciona bridge
    }
}
```

Depois C.bar() funciona ✅

---

## Conclusão

**BuLang NÃO faz busca automática em herança para `super` calls.**

O compilador gera `OP_SUPER_INVOKE(ownerClass)`, que procura em `ownerClass->superclass` apenas.

Se `ownerClass->superclass` não tiver o método, fails.

**Solução**: **Declare/override em CADA nível** da hierarquia.

Isto é diferente de:
- Java (virtual dispatch automático)
- Python (MRO automática)
- C++ (virtual function automática)

BuLang é **mais manual mas mais previsível**.

