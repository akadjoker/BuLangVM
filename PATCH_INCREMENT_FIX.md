# PATCH: Correção de Prefix/Postfix Increment/Decrement

## Problema
- `++i` e `--i` (prefix) não deixam valor na stack quando usados em expressões
- `i++` e `i--` (postfix) fazem OP_POP que remove o valor em expressões

## Solução
Os operadores devem SEMPRE retornar valor na stack:
- `++i` retorna novo valor
- `i++` retorna valor antigo
- O `expressionStatement()` já faz OP_POP para statements

## Mudanças Necessárias

### 1. Fix Postfix (i++, i--) em handle_assignment()
**Arquivo:** `libbu/src/compiler_statements.cpp`
**Linhas:** 464-483

**ANTES:**
```cpp
else if (match(TOKEN_PLUS_PLUS))
{
    // i++ (postfix)
    emitBytes(getOp, (uint8)arg);
    emitBytes(getOp, (uint8)arg);       // Duplica com GET GET (ineficiente)
    emitConstant(vm_->makeInt(1));
    emitByte(OP_ADD);
    emitBytes(setOp, (uint8)arg);
    emitByte(OP_POP);                    // <-- REMOVE O VALOR!
}
else if (match(TOKEN_MINUS_MINUS))
{
    // i-- (postfix)
    emitBytes(getOp, (uint8)arg);
    emitBytes(getOp, (uint8)arg);
    emitConstant(vm_->makeInt(1));
    emitByte(OP_SUBTRACT);
    emitBytes(setOp, (uint8)arg);
    emitByte(OP_POP);                    // <-- REMOVE O VALOR!
}
```

**DEPOIS:**
```cpp
else if (match(TOKEN_PLUS_PLUS))
{
    // i++ (postfix) - retorna valor ANTIGO
    emitBytes(getOp, (uint8)arg);        // [old_value]
    emitByte(OP_DUP);                    // [old_value, old_value] (mais eficiente)
    emitConstant(vm_->makeInt(1));       // [old_value, old_value, 1]
    emitByte(OP_ADD);                    // [old_value, new_value]
    emitBytes(setOp, (uint8)arg);        // [old_value] (SET consome new_value)
    // Deixa old_value na stack!
}
else if (match(TOKEN_MINUS_MINUS))
{
    // i-- (postfix) - retorna valor ANTIGO
    emitBytes(getOp, (uint8)arg);        // [old_value]
    emitByte(OP_DUP);                    // [old_value, old_value]
    emitConstant(vm_->makeInt(1));       // [old_value, old_value, 1]
    emitByte(OP_SUBTRACT);               // [old_value, new_value]
    emitBytes(setOp, (uint8)arg);        // [old_value]
    // Deixa old_value na stack!
}
```

### 2. Fix Prefix (++i, --i) em prefixIncrement/Decrement()
**Arquivo:** `libbu/src/compiler_statements.cpp`
**Linhas:** 1708-1810 e 1812-1907

**ANTES (prefixIncrement):**
```cpp
emitBytes(getOp, (uint8)arg);
emitConstant(vm_->makeInt(1));
emitByte(OP_ADD);
emitBytes(setOp, (uint8)arg);          // Não deixa valor!
```

**DEPOIS (prefixIncrement):**
```cpp
emitBytes(getOp, (uint8)arg);           // [old_value]
emitConstant(vm_->makeInt(1));          // [old_value, 1]
emitByte(OP_ADD);                       // [new_value]
emitByte(OP_DUP);                       // [new_value, new_value]
emitBytes(setOp, (uint8)arg);           // [new_value] (SET consome uma cópia)
// Deixa new_value na stack!
```

**ANTES (prefixDecrement):**
```cpp
emitBytes(getOp, (uint8)arg);
emitConstant(vm_->makeInt(1));
emitByte(OP_SUBTRACT);
emitBytes(setOp, (uint8)arg);          // Não deixa valor!
```

**DEPOIS (prefixDecrement):**
```cpp
emitBytes(getOp, (uint8)arg);           // [old_value]
emitConstant(vm_->makeInt(1));          // [old_value, 1]
emitByte(OP_SUBTRACT);                  // [new_value]
emitByte(OP_DUP);                       // [new_value, new_value]
emitBytes(setOp, (uint8)arg);           // [new_value]
// Deixa new_value na stack!
```

## Testes Para Validar

### Teste 1: Postfix em expressão
```javascript
var i = 5;
var x = i++;
print(x);  // Deve printar 5 (valor ANTES)
print(i);  // Deve printar 6 (foi incrementado)
```

### Teste 2: Prefix em expressão
```javascript
var i = 5;
var x = ++i;
print(x);  // Deve printar 6 (valor DEPOIS)
print(i);  // Deve printar 6
```

### Teste 3: Como statement (regressão)
```javascript
var i = 0;
while (i < 1000000) {
    ++i;  // Não deve vazar stack
}
print(i);  // Deve printar 1000000
```

### Teste 4: Combinado
```javascript
var a = 10;
var b = 20;
var result = (a++) + (++b);
print(a);      // 11
print(b);      // 21
print(result); // 10 + 21 = 31
```

## Nota sobre OP_SET_LOCAL/GLOBAL
Esta correção assume que:
- `OP_SET_LOCAL/GLOBAL` consome [value] da stack
- `OP_SET_LOCAL/GLOBAL` NÃO deixa valor na stack

Se o comportamento for diferente, ajustar conforme necessário.
