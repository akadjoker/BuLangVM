# BuLangVM - BuLang Syntax Quick Summary

## Porque o BuLang existe
Criei o BuLang porque, em projetos reais, Lua/Python acabam por exigir muito
trabalho de bindings e glue code para expor classes e sistemas do motor.
O objetivo do BuLang e reduzir esse custo: classes nativas e structs entram
direto no script, com menos friccao, menos boilerplate e mais velocidade no
fluxo de trabalho.

Em resumo:
- Menos tempo a escrever bindings
- Classes nativas simples de expor
- Script leve e direto para gameplay e tools

Este README e um resumo curto da sintaxe da BuLang, baseado nos exemplos e testes do repo.

**Tipos basicos**
- Numeros (int/float)
- Booleanos: `true`, `false`
- `nil`
- Strings: `"texto"`
- Verbatim string: `@"texto\nliteral"` (nao interpreta escapes)

**Variaveis**
```bu
var x = 10;
var nome = "Joao";
```

**Operadores**
- Aritmeticos: `+ - * / %`
- Comparacao: `== != < <= > >=`
- Logicos: `&& || !`
- Bitwise: `& | ^ ~ << >>`

**Strings**
```bu
print("ola");
print("tamanho =", "abc".length());
```

**Arrays**
```bu
var a = [1, 2, 3];
print(a[0]);
a[1] = 99;
```

**Maps**
```bu
var m = {"nome": "Joao", "idade": 25};
print(m["nome"]);
m["idade"] = 30;
```
Builtins principais do map:
- `has(chave)`
- `remove(chave)` (seta para `nil`)
- `clear()`
- `length()`
- `keys()`
- `values()`

**Buffers**
Criacao: `@(size, type)`
```bu
var buf = @(10, 0); // 10 bytes, UINT8
buf[0] = 42;
print(buf[0]);
print("len =", buf.length());
```
Tipos de buffer (indices):
- `0` = UINT8
- `1` = INT16
- `2` = UINT16
- `3` = INT32
- `4` = UINT32
- `5` = FLOAT
- `6` = DOUBLE

**Funcoes**
```bu
def soma(a, b) {
    return a + b;
}
print(soma(2, 3));
```

**Controle de fluxo**
```bu
if (x > 0) {
    print("pos");
} else {
    print("neg");
}

while (i < 10) {
    i = i + 1;
}

for (var n = 0; n < 5; n = n + 1) {
    if (n == 1) { continue; }
    if (n == 3) { break; }
}
```

**Structs e classes**
Existe suporte a `struct` e `class` no runtime. Ver exemplos em `scripts/`.

**Testes de sintaxe**
Veja:
- `scripts/test_syntax_master.bu`
- `scripts/test_at_disambiguation.bu`
