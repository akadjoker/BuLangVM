#include "interpreter.hpp"
#include "pool.hpp"
#include "opcode.hpp"
#include "debug.hpp"
#include <string>



void Interpreter::checkType(int index, ValueType expected, const char *funcName)
{
    Value v = peek(index);
    if (v.type != expected)
    {
        runtimeError("%s expects %s at index %d, got %s",
                     funcName,
                     valueTypeToString(expected),
                     index,
                     valueTypeToString(v.type));
    }
}



// int Interpreter::toInt(int index)
// {
//     checkType(index, ValueType::INT, "toInt");
//     return peek(index).asInt();
// }

// ===== STACK API =====

const Value &Interpreter::peek(int index)
{
    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");

    int top = getTop();
    int realIndex;

    if (index < 0)
        realIndex = top + index; // -1 → top-1, -2 → top-2
    else
        realIndex = index; // 0 → 0, 1 → 1

    if (realIndex < 0 || realIndex >= top)
    {
        runtimeError("Stack index %d out of bounds (size=%d)", index, top);
        static Value null = makeNil();
        return null;
    }

    return currentFiber->stack[realIndex];
}

int Interpreter::getTop()
{
    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");
    return static_cast<int>(currentFiber->stackTop - currentFiber->stack);
}

void Interpreter::setTop(int index)
{
    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");
    if (index < 0 || index > STACK_MAX)
    {
        runtimeError("Invalid stack index");
        return;
    }
    currentFiber->stackTop = currentFiber->stack + index;
}

void Interpreter::push(Value value)
{

    if (currentFiber->stackTop >= currentFiber->stack + STACK_MAX)
    {
        runtimeError("Stack overflow");
        return;
    }
    *currentFiber->stackTop++ = value;
}

Value Interpreter::pop()
{

    if (currentFiber->stackTop <= currentFiber->stack)
    {
        runtimeError("Stack underflow");
        return makeNil();
    }
    return *--currentFiber->stackTop;
}

//  const Value &Interpreter::peek(int distance)
// {

//     //    int stackSize = currentFiber->stackTop - currentFiber->stack;
//     ptrdiff_t stackSize = currentFiber->stackTop - currentFiber->stack;

//     if (distance < 0 || distance >= stackSize)
//     {
//         runtimeError("Stack peek out of bounds: distance=%d, size=%d",
//                      distance, stackSize);
//         static const Value null = makeNil();
//         return null;
//     }
//     return currentFiber->stackTop[-1 - distance];
// }

// Type checking
ValueType Interpreter::getType(int index)
{
    return peek(index).type;
}

bool Interpreter::isInt(int index)
{
    return peek(index).type == ValueType::INT;
}

bool Interpreter::isDouble(int index)
{
    return peek(index).type == ValueType::DOUBLE;
}

bool Interpreter::isString(int index)
{
    return peek(index).type == ValueType::STRING;
}

bool Interpreter::isBool(int index)
{
    return peek(index).type == ValueType::BOOL;
}

bool Interpreter::isNil(int index)
{
    return peek(index).type == ValueType::NIL;
}

bool Interpreter::isFunction(int index)
{
    return peek(index).type == ValueType::FUNCTION;
}

void Interpreter::pushInt(int n)
{
    push(makeInt(n));
}

void Interpreter::pushFloat(float f)
{
    push(makeDouble(f));
}

void Interpreter::pushPointer(void *p)
{
    push(makePointer(p));
}

void Interpreter::pushByte(uint8 b)
{
    push(makeInt(b));
}

void Interpreter::pushDouble(double d)
{
    push(makeDouble(d));
}

void Interpreter::pushString(const char *s)
{
    push(makeString(s));
}

void Interpreter::pushBool(bool b)
{
    push(makeBool(b));
}

void Interpreter::pushNil()
{
    push(makeNil());
}

// Type conversions
int Interpreter::toInt(int index)
{
    Value v = peek(index);
    if (!v.isInt())
    {
        runtimeError("Expected int at index %d", index);
        return 0;
    }
    return v.asInt();
}

double Interpreter::toDouble(int index)
{
    Value v = peek(index);

    if (v.isDouble())
    {
        return v.asDouble();
    }
    else if (v.isInt())
    {
        return (double)v.asInt();
    }

    runtimeError("Expected number at index %d", index);
    return 0.0;
}

const char *Interpreter::toString(int index)
{
    const Value &v = peek(index);
    if (!v.isString())
    {
        runtimeError("Expected string at index %d", index);
        return "";
    }
    return v.asString()->chars();
}

bool Interpreter::toBool(int index)
{
    Value v = peek(index);
    return isTruthy(v);
}

void Interpreter::insert(int index)
{
    // Insere topo no index, shift elementos
    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");

    int top = getTop();
    if (index < 0)
        index = top + index + 1;
    if (index < 0 || index > top)
    {
        runtimeError("Invalid insert index");
        return;
    }

    Value value = pop();

    // Shift elementos para direita
    for (int i = top - 1; i >= index; i--)
    {
        currentFiber->stack[i + 1] = currentFiber->stack[i];
    }

    currentFiber->stack[index] = value;
    currentFiber->stackTop++;
}

void Interpreter::remove(int index)
{
    // Remove elemento no index
    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");

    int top = getTop();
    if (index < 0)
        index = top + index;
    if (index < 0 || index >= top)
    {
        runtimeError("Invalid remove index");
        return;
    }

    // Shift elementos para esquerda
    for (int i = index; i < top - 1; i++)
    {
        currentFiber->stack[i] = currentFiber->stack[i + 1];
    }

    currentFiber->stackTop--;
}

void Interpreter::replace(int index)
{

    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");

    int top = getTop();
    if (index < 0)
        index = top + index;
    if (index < 0 || index >= top)
    {
        runtimeError("Invalid replace index");
        return;
    }

    currentFiber->stack[index] = pop();
}
void Interpreter::copy(int fromIndex, int toIndex)
{

    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");

    int top = getTop();
    if (fromIndex < 0)
        fromIndex = top + fromIndex;
    if (toIndex < 0)
        toIndex = top + toIndex;

    if (fromIndex < 0 || fromIndex >= top ||
        toIndex < 0 || toIndex >= top)
    {
        runtimeError("Invalid copy indices");
        return;
    }

    currentFiber->stack[toIndex] = currentFiber->stack[fromIndex];
}

void Interpreter::rotate(int index, int n)
{
    // Roda n elementos a partir de index
    // rotate(-3, 1): ABC → CAB
    WDIV_ASSERT(currentFiber != nullptr, "No current fiber");

    int top = getTop();
    if (index < 0)
        index = top + index;

    if (index < 0 || index >= top || n == 0)
        return;

    int count = top - index;
    n = ((n % count) + count) % count;

    // Reverse [index, index+count-n)
    for (int i = 0; i < (count - n) / 2; i++)
    {
        Value temp = currentFiber->stack[index + i];
        currentFiber->stack[index + i] = currentFiber->stack[index + count - n - 1 - i];
        currentFiber->stack[index + count - n - 1 - i] = temp;
    }

    // Reverse [index+count-n, index+count)
    for (int i = 0; i < n / 2; i++)
    {
        Value temp = currentFiber->stack[index + count - n + i];
        currentFiber->stack[index + count - n + i] = currentFiber->stack[index + count - 1 - i];
        currentFiber->stack[index + count - 1 - i] = temp;
    }

    // Reverse [index, index+count)
    for (int i = 0; i < count / 2; i++)
    {
        Value temp = currentFiber->stack[index + i];
        currentFiber->stack[index + i] = currentFiber->stack[index + count - 1 - i];
        currentFiber->stack[index + count - 1 - i] = temp;
    }
}

bool Interpreter::callFunction(Function *func, int argCount)
{
    if (!func)
    {
        runtimeError("Cannot call null function");
        return false;
    }

    // Verifica arity
    if (argCount != func->arity)
    {
        runtimeError("Function '%s' expects %d arguments but got %d",
                     func->name->chars(), func->arity, argCount);
        return false;
    }

    if (!currentFiber)
    {
        runtimeError("No active fiber to call function");
        return false;
    }

    // Verifica overflow de frames
    if (currentFiber->frameCount >= FRAMES_MAX)
    {
        runtimeError("Stack overflow - too many nested calls");
        return false;
    }

    if (!func->chunk || func->chunk->count == 0)
    {
        runtimeError("Function '%s' has no bytecode!", func->name->chars());
        return false;
    }

//    Debug::disassembleChunk(*func->chunk, func->name->chars());

    CallFrame *frame = &currentFiber->frames[currentFiber->frameCount];
    frame->func = func;
    frame->ip = func->chunk->code;

    frame->slots = currentFiber->stackTop - argCount;

    currentFiber->frameCount++;

    int targetFrames = currentFiber->frameCount - 1;

    while (currentFiber->frameCount > targetFrames)
    {
        FiberResult result = run_fiber(currentFiber,currentProcess);

        if (result.reason == FiberResult::ERROR)
        {
            return false;
        }
    }

    return true;
}

bool Interpreter::callFunction(const char *name, int argCount)
{
    // Lookup function por nome
    String *funcName = createString(name);
    Function *func = nullptr;

    if (!functionsMap.get(funcName, &func))
    {

        runtimeError("Undefined function: %s", name);
        return false;
    }


    return callFunction(func, argCount);
}

Function* Interpreter::getFunction(const char *name)
{
    // Tentar nome directo primeiro
    String *funcName = createString(name);
    Function *func = nullptr;

    if (functionsMap.get(funcName, &func))
    {
        return func;
    }

    // Tentar com prefixo __main__$
    char prefixedName[256];
    snprintf(prefixedName, sizeof(prefixedName), "__main__$%s", name);

    String *prefixedString = createString(prefixedName);
    if (functionsMap.get(prefixedString, &func))
    {
        return func;
    }

    return nullptr;
}

bool Interpreter::callFunctionAuto(const char *name, int argCount)
{
    Function *func = getFunction(name);

    if (!func)
    {
        runtimeError("Undefined function: %s", name);
        return false;
    }

    return callFunction(func, argCount);
}

Process *Interpreter::callProcess(ProcessDef *proc, int argCount)
{
    if (!proc)
    {
        runtimeError("Cannot call null process");
        return nullptr;
    }

    Function *processFunc = proc->fibers[0].frames[0].func;

    // Verifica arity
    if (argCount != processFunc->arity)
    {
        runtimeError("Process '%s' expects %d arguments but got %d",
                     proc->name->chars(), processFunc->arity, argCount);
        return nullptr;
    }

    // Spawn process
    Process *instance = spawnProcess(proc);

    if (!instance)
    {
        runtimeError("Failed to spawn process");
        return nullptr;
    }

    // Se tem argumentos, inicializa
    if (argCount > 0)
    {
        Fiber *procFiber = &instance->fibers[0];

        // Copia args da stack atual para a fiber do processo
        for (int i = 0; i < argCount; i++)
        {
            procFiber->stack[i] = currentFiber->stackTop[-argCount + i];
        }
        procFiber->stackTop = procFiber->stack + argCount;

        // Mapeia args para privates (se definido)
        for (size_t i = 0; i < proc->argsNames.size() && i < (size_t)argCount; i++)
        {
            uint8_t privIdx = proc->argsNames[i];
            if (privIdx != 255)
            {
                instance->privates[privIdx] = procFiber->stack[i];
            }
        }

        // Remove args da stack atual
        currentFiber->stackTop -= argCount;
    }

    // ID e FATHER
    instance->privates[(int)PrivateIndex::ID] = makeInt(instance->id);
    if (currentProcess && currentProcess->id > 0)
    {
        instance->privates[(int)PrivateIndex::FATHER] = makeInt(currentProcess->id);
    }

    if (hooks.onStart)
    {
        hooks.onStart(instance);
    }

    return instance;
}

Process *Interpreter::callProcess(const char *name, int argCount)
{
    String *procName = createString(name);
    ProcessDef *proc = nullptr;

    if (!processesMap.get(procName, &proc))
    {
       
        runtimeError("Undefined process: %s", name);
        return nullptr;
    }

 
    return callProcess(proc, argCount);
}
