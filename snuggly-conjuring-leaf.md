# BuLang VM Lite - Plano de Implementação

Branch `lite` a partir de `main`. Remove processos, fibers múltiplas, e scheduling. Mantém: funções, classes, structs, native bindings, modules, GC, closures.

## Resumo das Alterações

| Ficheiro | Acção |
|----------|-------|
| `libbu/include/interpreter.hpp` | Remover Process/ProcessDef structs, VMHooks, membros de processo. Adicionar `Fiber mainFiber` |
| `libbu/include/pool.hpp` + `pool.cpp` | Remover ProcessPool |
| `libbu/include/opcode.hpp` | Marcar OP_SPAWN/OP_FRAME/OP_EXIT/OP_GET_PRIVATE/OP_SET_PRIVATE como unused |
| `libbu/include/token.hpp` + `lexer.cpp` + `token.cpp` | Remover TOKEN_PROCESS/TOKEN_FIBER/TOKEN_FRAME/TOKEN_EXIT |
| `libbu/src/compiler.cpp` | `compile()` retorna `Function*` em vez de `ProcessDef*` |
| `libbu/src/compiler_statements.cpp` | Remover processDeclaration, fiberStatement, frameStatement, exitStatement |
| `libbu/src/interpreter.cpp` | Simplificar `run()`, `reset()`, `createClassInstance()` para usar `mainFiber` directamente |
| `libbu/src/interpreter_process.cpp` | Esvaziar (ou eliminar) - tudo é processo-específico |
| `libbu/src/interpreter_runtime_switch.cpp` | Mudar `run_fiber(Fiber*)`, remover 24 refs a `process->` |
| `libbu/src/interpreter_runtime_goto.cpp` | Mesmas alterações que switch |
| `libbu/src/interpreter_stack.cpp` | `callFunction`/`callMethod` usam `mainFiber`, remover `callProcess` |
| `libbu/src/gc.cpp` | `markRoots()` marca só `mainFiber` em vez de iterar processos |
| `libbu/src/value.cpp` + `value.hpp` | Remover ValueType::PROCESS |
| `libbu/src/debug.cpp` | Remover disassemble de process privates |

## Passos Ordenados (compilável a cada passo)

### Passo 1: Branch e constantes
- `git checkout -b lite main`
- Em `opcode.hpp`: renomear OP_SPAWN/OP_FRAME/OP_EXIT/OP_GET_PRIVATE/OP_SET_PRIVATE para OP_UNUSED_XX (manter slots numéricos)

### Passo 2: Remover tokens de processo do lexer
- Em `token.hpp`: remover TOKEN_PROCESS, TOKEN_FIBER, TOKEN_FRAME, TOKEN_EXIT
- Em `lexer.cpp`/`token.cpp`: remover da keyword table

### Passo 3: Simplificar o compiler
- `compiler.hpp`: `compile()` retorna `Function*`, remover `isProcess_`, `numFibers_`, `currentProcess` (ProcessDef*)
- `compiler.cpp`: `compile()` cria a __main__ Function e retorna-a directamente, sem addProcess()
- `compiler_statements.cpp`: remover `processDeclaration()`, `fiberStatement()`, `frameStatement()`, `exitStatement()`, e os branches que os chamam em `declaration()` e `statement()`

### Passo 4: Limpar headers
- `interpreter.hpp`:
  - Remover structs: `ProcessDef`, `Process`, `PrivateIndex`, `VMHooks`
  - Remover do Interpreter: `processesMap`, `privateIndexMap`, `processes`, `aliveProcesses`, `cleanProcesses`, timing fields, `currentProcess`, `mainProcess`, `hooks`
  - Remover métodos: `addProcess`, `spawnProcess`, `destroyProcess`, `callProcess`, `setHooks`, `update`, `render`, `run_process_step`, `get_ready_fiber`, `setPrivateTable`, `getProcessPrivateIndex`, `addFiber`, `getCurrentTime`, `freeRunningProcesses`, `getTotalProcesses`, `getTotalAliveProcesses`, `getCurrentProcess`, `setCurrentProcess`, `makeProcess`
  - Mudar: `run_fiber(Fiber*, Process*)` → `run_fiber(Fiber*)`
  - Adicionar: `Fiber mainFiber;` como membro privado
- `pool.hpp`/`pool.cpp`: remover ProcessPool
- `value.hpp`/`value.cpp`: remover ValueType::PROCESS e helpers

### Passo 5: Simplificar interpreter.cpp
- `run()`: `reset()` → `compile()` → `initFiber(&mainFiber, func)` → `currentFiber = &mainFiber` → `run_fiber(&mainFiber)`
- `compile(source, dump)`: mesma simplificação
- `reset()`: remover freeRunningProcesses, resetar mainFiber
- `createClassInstance()`: usar `&mainFiber` em vez de `mainProcess->fibers[0]`, chamar `run_fiber(fiber)` sem Process
- `createClassInstanceRaw()`: idem (sem constructor call)
- Remover: `freeRunningProcesses`, `setPrivateTable`, `getProcessPrivateIndex`, `addFiber`
- `freeBlueprints()`: remover loop de processes

### Passo 6: Esvaziar interpreter_process.cpp
- Remover todo o conteúdo (todas as funções são processo-específicas)
- Ou eliminar o ficheiro e actualizar CMakeLists

### Passo 7: Adaptar runtime dispatch (switch + goto)
- Mudar signature: `run_fiber(Fiber *fiber)` (remover Process*)
- **OP_GET_PRIVATE/OP_SET_PRIVATE** (switch:306,313): runtime error
- **OP_YIELD** (switch:1402): manter como está (retorna FIBER_YIELD)
- **OP_FRAME** (switch:1413): runtime error
- **OP_EXIT** (switch:1422): runtime error
- **OP_SPAWN** (switch:1449): runtime error
- **OP_RETURN** (switch:1281,1374,4599): remover blocos `if (fiber == &process->fibers[0])`, simplificar para `fiber->state = DEAD; return FIBER_DONE` quando frameCount == 0
- **OP_CALL com isProcess()** (switch:~956): runtime error
- Aplicar mesmas alterações ao ficheiro goto

### Passo 8: Simplificar GC
- `gc.cpp markRoots()`: substituir iteração de `aliveProcesses` por marcação directa de `mainFiber.stack` + `mainFiber.frames[].closure`

### Passo 9: Simplificar interpreter_stack.cpp
- `callFunction()`: usar `run_fiber(currentFiber)` sem Process
- `callMethod()`: usar `&mainFiber` em vez de `mainProcess->fibers[0]`, chamar `run_fiber(fiber)`
- Remover `callProcess()` (ambos overloads)

### Passo 10: Limpeza
- `debug.cpp`: remover dumps de privates/process
- Verificar que `bu/src/main.cpp` compila (remover VMHooks setup)
- Remover `FiberResult::PROCESS_FRAME` do enum

## Verificação

1. `cmake --build build` - compila sem erros
2. `./bin/testCBiding bidings.bu` - teste básico de bindings (Point, Vector2D)
3. `./bin/testCBiding --gameobject` - teste GameObjects com ScriptComponent
4. Scripts com classes, herança, closures, try/catch, modules funcionam
5. Scripts que usem `process`, `fiber`, `frame`, `exit` dão erro de compilação (expected)

## Estimativa de Scope

- ~15 ficheiros a modificar
- ~24 referências a `process->` no runtime switch (e similar no goto)
- Maioria é remoção de código, não escrita de código novo
- O core da VM (opcodes, stack, GC) mantém-se intacto
