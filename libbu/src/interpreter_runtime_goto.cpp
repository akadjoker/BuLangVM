#include "interpreter.hpp"
#include "pool.hpp"
#include "opcode.hpp"
#include "debug.hpp"
#include <cmath> // std::fmod
#include <new>

#ifdef USE_COMPUTED_GOTO

#define DEBUG_TRACE_EXECUTION 0 // 1 = ativa, 0 = desativa
#define DEBUG_TRACE_STACK 0     // 1 = mostra stack, 0 = esconde

const char *getOpcodeName(uint8 op)
{
    static const char *names[] = {
        "OP_CONSTANT", "OP_NIL", "OP_TRUE", "OP_FALSE",
        "OP_POP", "OP_HALT", "OP_NOT", "OP_DUP",
        "OP_ADD", "OP_SUBTRACT", "OP_MULTIPLY", "OP_DIVIDE", "OP_NEGATE", "OP_MODULO",
        "OP_BITWISE_AND", "OP_BITWISE_OR", "OP_BITWISE_XOR", "OP_BITWISE_NOT", "OP_SHIFT_LEFT", "OP_SHIFT_RIGHT",
        "OP_EQUAL", "OP_NOT_EQUAL", "OP_GREATER", "OP_GREATER_EQUAL", "OP_LESS", "OP_LESS_EQUAL",
        "OP_GET_LOCAL", "OP_SET_LOCAL", "OP_GET_GLOBAL", "OP_SET_GLOBAL", "OP_DEFINE_GLOBAL", "OP_GET_PRIVATE", "OP_SET_PRIVATE",
        "OP_JUMP", "OP_JUMP_IF_FALSE", "OP_LOOP", "OP_GOSUB", "OP_RETURN_SUB",
        "OP_CALL", "OP_RETURN", "OP_SPAWN", "OP_YIELD", "OP_FRAME", "OP_EXIT",
        "OP_DEFINE_ARRAY", "OP_DEFINE_MAP",
        "OP_GET_PROPERTY", "OP_SET_PROPERTY", "OP_GET_INDEX", "OP_SET_INDEX",
        "OP_INVOKE", "OP_SUPER_INVOKE",
        "OP_PRINT", "OP_LEN", "OP_FOREACHSTART,", "OP_FOREACHEAK", "OP_FOREACHNEXT"};
    return (op <= 52) ? names[op] : "UNKNOWN";
}

bool toNumberPair(const Value &a, const Value &b, double &da, double &db)
{
    if (!(a.isInt() || a.isDouble()))
        return false;
    if (!(b.isInt() || b.isDouble()))
        return false;

    da = a.isInt() ? static_cast<double>(a.asInt()) : a.asDouble();
    db = b.isInt() ? static_cast<double>(b.asInt()) : b.asDouble();
    return true;
}

FiberResult Interpreter::run_fiber(Fiber *fiber)
{

    currentFiber = fiber;

    CallFrame *frame;
    Value *stackStart;
    uint8 *ip;
    Function *func;

    int instructionsRun = 0;

// Macros
#define DROP() (fiber->stackTop--)
#define PEEK() (*(fiber->stackTop - 1))
#define PEEK2() (*(fiber->stackTop - 2))
#define POP() (*(--fiber->stackTop))
#define PUSH(value) (*fiber->stackTop++ = value)
#define NPEEK(n) (fiber->stackTop[-1 - (n)])
#define READ_BYTE() (*ip++)
#define READ_SHORT() (ip += 2, (uint16)((ip[-2] << 8) | ip[-1]))
#define READ_CONSTANT() (func->chunk->constants[READ_BYTE()])

#define BINARY_OP_PREP()           \
    Value b = fiber->stackTop[-1]; \
    Value a = fiber->stackTop[-2]; \
    fiber->stackTop -= 2

#define STORE_FRAME() frame->ip = ip

#define LOAD_FRAME()                                   \
    do                                                 \
    {                                                  \
        assert(currentFiber->frameCount > 0);          \
        frame = &fiber->frames[fiber->frameCount - 1]; \
        stackStart = frame->slots;                     \
        ip = frame->ip;                                \
        func = frame->func;                            \
    } while (false)

    static const void *dispatch_table[] = {
        // Literals (0-3)
        &&op_constant, &&op_nil, &&op_true, &&op_false,

        // Stack (4-7)
        &&op_pop, &&op_halt, &&op_not, &&op_dup,

        // Arithmetic (8-13)
        &&op_add, &&op_subtract, &&op_multiply, &&op_divide, &&op_negate, &&op_modulo,

        // Bitwise (14-19)
        &&op_bitwise_and, &&op_bitwise_or, &&op_bitwise_xor, &&op_bitwise_not, &&op_shift_left, &&op_shift_right,

        // Comparisons (20-25)
        &&op_equal, &&op_not_equal, &&op_greater, &&op_greater_equal, &&op_less, &&op_less_equal,

        // Variables (26-32)
        &&op_get_local, &&op_set_local, &&op_get_global, &&op_set_global, &&op_define_global, &&op_get_private, &&op_set_private,

        // Control flow (33-37)
        &&op_jump, &&op_jump_if_false, &&op_loop, &&op_gosub, &&op_return_sub,

        // Functions (38-43)
        &&op_call, &&op_return, &&op_spawn, &&op_yield, &&op_frame, &&op_exit,

        // Collections (44-45)
        &&op_define_array, &&op_define_map,

        // Properties (46-49)
        &&op_get_property, &&op_set_property, &&op_get_index, &&op_set_index,

        // Methods (50-51)
        &&op_invoke, &&op_super_invoke,

        // I/O (52)
        &&op_print,
        // inter functions
        &&op_len,
        // forech
        &&op_foreach_start, &&op_foreach_next, &&op_foreach_check};

    // #define DISPATCH()                                    \
    //     do                                                \
    //     {                                                 \
    //         instructionsRun++;                            \
    //         uint8 nextOp = *ip;                           \
    //         printf("[DISPATCH] offset=%ld, opcode=%d (%s), ip=%p\n", \
    //                (long)(ip - func->chunk->code),        \
    //                nextOp,                                \
    //                getOpcodeName(nextOp),                 \
    //                (void*)ip);                            \
    //         goto *dispatch_table[READ_BYTE()];            \
    //     } while (0)

#define DISPATCH()                         \
    do                                     \
    {                                      \
        instructionsRun++;                 \
        uint8 nextOp = *ip;                \
        goto *dispatch_table[READ_BYTE()]; \
    } while (0)

    LOAD_FRAME();

    DISPATCH();

op_constant:
{
    const Value &constant = READ_CONSTANT();
    PUSH(constant);
    DISPATCH();
}

op_nil:
{
    PUSH(makeNil());
    DISPATCH();
}

op_true:
{
    PUSH(makeBool(true));
    DISPATCH();
}

op_false:
{
    PUSH(makeBool(false));
    DISPATCH();
}

op_dup:
{
    PUSH(PEEK());
    DISPATCH();
}

    // ========== STACK MANIPULATION ==========

op_pop:
{
    POP();
    DISPATCH();
}

op_halt:
{
    return {FiberResult::FIBER_DONE, (int)instructionsRun, 0, 0};
}

    // ========== VARIABLES ==========

op_get_local:
{
    uint8 slot = READ_BYTE();

    PUSH(stackStart[slot]);
    DISPATCH();
}

op_set_local:
{
    uint8 slot = READ_BYTE();
    stackStart[slot] = PEEK();
    DISPATCH();
}

op_get_private:
{
    uint8 index = READ_BYTE();
    PUSH(currentProcess->privates[index]);
    DISPATCH();
}

op_set_private:
{
    uint8 index = READ_BYTE();
    currentProcess->privates[index] = PEEK();
    DISPATCH();
}

op_get_global:
{

    const Value &name = READ_CONSTANT();
    Value value;

    if (!globals.get(name.asString(), &value))
    {
        runtimeError("Undefined variable '%s'", name.asString()->chars());
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(value);
    DISPATCH();
}

op_set_global:
{

    const Value &name = READ_CONSTANT();
    const Value &value = PEEK();
    if (globals.set(name.asString(), value))
    {
    }
    DISPATCH();
}

op_define_global:
{

    const Value &name = READ_CONSTANT();
    globals.set(name.asString(), POP());
    DISPATCH();
}

    // ========== ARITHMETIC ==========

// ============================================
// OP_ADD
// ============================================
op_add:
{
    BINARY_OP_PREP();

    // String concatenation
    if (a.isString() && b.isString())
    {
        String *result = stringPool.concat(a.asString(), b.asString());
        PUSH(makeString(result));
        DISPATCH();
    }
    if (a.isString() && b.isDouble())
    {
        String *right = stringPool.toString(b.asDouble());
        String *result = stringPool.concat(a.asString(), right);
        PUSH(makeString(result));
        DISPATCH();
    }
    if (a.isString() && b.isInt())
    {
        String *right = stringPool.toString(b.asInt());
        String *result = stringPool.concat(a.asString(), right);
        PUSH(makeString(result));
        DISPATCH();
    }

    // Numeric operations
    if (a.isInt() && b.isInt())
    {
        PUSH(makeInt(a.asInt() + b.asInt()));
        DISPATCH();
    }
    if (a.isInt() && b.isDouble())
    {
        PUSH(makeDouble(a.asInt() + b.asDouble()));
        DISPATCH();
    }
    if (a.isDouble() && b.isInt())
    {
        PUSH(makeDouble(a.asDouble() + b.asInt()));
        DISPATCH();
    }
    if (a.isDouble() && b.isDouble())
    {
        PUSH(makeDouble(a.asDouble() + b.asDouble()));
        DISPATCH();
    }

    runtimeError("Operands '+' must be numbers or strings");
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

// ============================================
// OP_SUBTRACT
// ============================================
op_subtract:
{
    BINARY_OP_PREP();

    if (a.isInt() && b.isInt())
    {
        PUSH(makeInt(a.asInt() - b.asInt()));
        DISPATCH();
    }
    if (a.isInt() && b.isDouble())
    {
        PUSH(makeDouble(a.asInt() - b.asDouble()));
        DISPATCH();
    }
    if (a.isDouble() && b.isInt())
    {
        PUSH(makeDouble(a.asDouble() - b.asInt()));
        DISPATCH();
    }
    if (a.isDouble() && b.isDouble())
    {
        PUSH(makeDouble(a.asDouble() - b.asDouble()));
        DISPATCH();
    }

    runtimeError("Operands '-' must be numbers");
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

// ============================================
// OP_MULTIPLY
// ============================================
op_multiply:
{
    BINARY_OP_PREP();

    if (a.isInt() && b.isInt())
    {
        PUSH(makeInt(a.asInt() * b.asInt()));
        DISPATCH();
    }
    if (a.isInt() && b.isDouble())
    {
        PUSH(makeDouble(a.asInt() * b.asDouble()));
        DISPATCH();
    }
    if (a.isDouble() && b.isInt())
    {
        PUSH(makeDouble(a.asDouble() * b.asInt()));
        DISPATCH();
    }
    if (a.isDouble() && b.isDouble())
    {
        PUSH(makeDouble(a.asDouble() * b.asDouble()));
        DISPATCH();
    }

    runtimeError("Operands must be numbers");
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

// ============================================
// OP_DIVIDE
// ============================================
op_divide:
{
    BINARY_OP_PREP();

    //  retorna double (divisão!)
    if (a.isInt() && b.isInt())
    {
        if (b.asInt() == 0)
        {
            runtimeError("Division by zero");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        PUSH(makeDouble((double)a.asInt() / b.asInt())); //  Double!
        DISPATCH();
    }
    if (a.isInt() && b.isDouble())
    {
        if (b.asDouble() == 0.0)
        {
            runtimeError("Division by zero");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        PUSH(makeDouble(a.asInt() / b.asDouble()));
        DISPATCH();
    }
    if (a.isDouble() && b.isInt())
    {
        if (b.asInt() == 0)
        {
            runtimeError("Division by zero");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        PUSH(makeDouble(a.asDouble() / b.asInt()));
        DISPATCH();
    }
    if (a.isDouble() && b.isDouble())
    {
        if (b.asDouble() == 0.0)
        {
            runtimeError("Division by zero");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        PUSH(makeDouble(a.asDouble() / b.asDouble()));
        DISPATCH();
    }

    runtimeError("Operands must be numbers");
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

// ============================================
// OP_MODULO
// ============================================
op_modulo:
{
    BINARY_OP_PREP();

    if (a.isInt() && b.isInt())
    {
        if (b.asInt() == 0)
        {
            runtimeError("Modulo by zero");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        PUSH(makeInt(a.asInt() % b.asInt()));
        DISPATCH();
    }

    // Para doubles, usa fmod
    double da = a.isInt() ? a.asInt() : a.asDouble();
    double db = b.isInt() ? b.asInt() : b.asDouble();

    if (db == 0.0)
    {
        runtimeError("Modulo by zero");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    PUSH(makeDouble(fmod(da, db)));
    DISPATCH();
}

    //======== LOGICAL =====

op_negate:
{
    Value a = POP();
    if (a.isInt())
    {
        PUSH(makeInt(-a.asInt()));
    }
    else if (a.isDouble())
    {
        PUSH(makeDouble(-a.asDouble()));
    }
    else if (a.isBool())
    {
        PUSH(makeBool(!a.asBool()));
    }
    else
    {
        runtimeError("Operand 'NEGATE' must be a number");
        printValueNl(a);
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    DISPATCH();
}

op_equal:
{
    BINARY_OP_PREP();
    PUSH(makeBool(valuesEqual(a, b)));

    DISPATCH();
}

op_not:
{
    Value v = POP();
    PUSH(makeBool(!isTruthy(v)));
    DISPATCH();
}

op_not_equal:
{
    BINARY_OP_PREP();
    PUSH(makeBool(!valuesEqual(a, b)));
    DISPATCH();
}

op_greater:
{
    BINARY_OP_PREP();

    double da, db;
    if (!toNumberPair(a, b, da, db))
    {
        runtimeError("Operands '>' must be numbers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    PUSH(makeBool(da > db));
    DISPATCH();
}

op_greater_equal:
{
    BINARY_OP_PREP();

    double da, db;
    if (!toNumberPair(a, b, da, db))
    {
        runtimeError("Operands '>=' must be numbers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeBool(da >= db));
    DISPATCH();
}

op_less:
{
    BINARY_OP_PREP();

    double da, db;
    if (!toNumberPair(a, b, da, db))
    {

        runtimeError("Operands '<' must be numbers");
        printValueNl(a);
        printValueNl(b);

        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeBool(da < db));
    DISPATCH();
}

op_less_equal:
{
    BINARY_OP_PREP();
    double da, db;
    if (!toNumberPair(a, b, da, db))
    {
        runtimeError("Operands  '<=' must be numbers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeBool(da <= db));
    DISPATCH();
}

    // ======= BITWISE =====

op_bitwise_and:
{
    BINARY_OP_PREP();
    if (!a.isInt() || !b.isInt())
    {
        runtimeError("Bitwise AND requires integers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeInt(a.asInt() & b.asInt()));
    DISPATCH();
}

op_bitwise_or:
{
    BINARY_OP_PREP();
    if (!a.isInt() || !b.isInt())
    {
        runtimeError("Bitwise OR requires integers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeInt(a.asInt() | b.asInt()));
    DISPATCH();
}

op_bitwise_xor:
{
    BINARY_OP_PREP();
    if (!a.isInt() || !b.isInt())
    {
        runtimeError("Bitwise XOR requires integers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeInt(a.asInt() ^ b.asInt()));
    DISPATCH();
}

op_bitwise_not:
{
    Value a = POP();
    if (!a.isInt())
    {
        runtimeError("Bitwise NOT requires integer");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeInt(~a.asInt()));
    DISPATCH();
}

op_shift_left:
{
    BINARY_OP_PREP();
    if (!a.isInt() || !b.isInt())
    {
        runtimeError("Shift left requires integers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeInt(a.asInt() << b.asInt()));
    DISPATCH();
}

op_shift_right:
{
    BINARY_OP_PREP();
    if (!a.isInt() || !b.isInt())
    {
        runtimeError("Shift right requires integers");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    PUSH(makeInt(a.asInt() >> b.asInt()));
    DISPATCH();
}

    // ========== CONTROL FLOW ==========

op_jump:
{
    uint16 offset = READ_SHORT();
    ip += offset;
    DISPATCH();
}

op_jump_if_false:
{
    uint16 offset = READ_SHORT();
    if (isFalsey(PEEK()))
        ip += offset;
    DISPATCH();
}

op_loop:
{
    uint16 offset = READ_SHORT();

    ip -= offset;

    DISPATCH();
}

    // ========== FUNCTIONS ==========

op_call:
{
    uint8 argCount = READ_BYTE();

    //  SALVA IP ATUAL!
    STORE_FRAME();

    Value callee = NPEEK(argCount);

    // ========================================
    // PATH 1: FUNCTION
    // ========================================
    if (callee.isFunction())
    {
        int index = callee.asFunctionId();
        Function *targetFunc = functions[index];

        if (!targetFunc)
        {
            runtimeError("Invalid function");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        if (argCount != targetFunc->arity)
        {
            runtimeError("Function %s expected %d arguments but got %d",
                         targetFunc->name->chars(), targetFunc->arity, argCount);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        if (fiber->frameCount >= FRAMES_MAX)
        {
            runtimeError("Stack overflow");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        CallFrame *newFrame = &fiber->frames[fiber->frameCount++];
        newFrame->func = targetFunc;
        newFrame->ip = targetFunc->chunk->code;
        newFrame->slots = fiber->stackTop - argCount - 1;

        //  Criou frame! Reload!
        LOAD_FRAME();
        DISPATCH();
    }

    // ========================================
    // PATH 2: NATIVE
    // ========================================
    else if (callee.isNative())
    {
        int index = callee.asNativeId();
        NativeDef nativeFunc = natives[index];

        if (nativeFunc.arity != -1 && argCount != nativeFunc.arity)
        {
            runtimeError("Function %s expected %d arguments but got %d",
                         nativeFunc.name->chars(), nativeFunc.arity, argCount);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Value result = nativeFunc.func(this, argCount, fiber->stackTop - argCount);
        fiber->stackTop -= (argCount + 1);
        PUSH(result);

        //  Não criou frame!
        DISPATCH();
    }

    // ========================================
    // PATH 3: PROCESS
    // ========================================
    else if (callee.isProcess())
    {
        int index = callee.asProcessId();
        ProcessDef *blueprint = processes[index];

        if (!blueprint)
        {
            runtimeError("Invalid process");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Function *processFunc = blueprint->fibers[0].frames[0].func;

        if (argCount != processFunc->arity)
        {
            runtimeError("Process expected %d arguments but got %d",
                         processFunc->arity, argCount);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // SPAWN - clona blueprint
        Process *instance = spawnProcess(blueprint);

        // Se tem argumentos, inicializa locals da fiber
        if (argCount > 0)
        {
            Fiber *procFiber = &instance->fibers[0];

            for (int i = 0; i < argCount; i++)
            {
                procFiber->stack[i] = fiber->stackTop[-(argCount - i)];
                if (blueprint->argsNames.size() > 0)
                {
                    uint8 idx = blueprint->argsNames[i];
                    if (idx != 255)
                    {
                        instance->privates[idx] = procFiber->stack[i];
                    }
                }
            }

            procFiber->stackTop = procFiber->stack + argCount;
        }

        // Remove callee + args da stack atual
        fiber->stackTop -= (argCount + 1);

        instance->privates[(int)PrivateIndex::ID] = makeInt(instance->id);
        instance->privates[(int)PrivateIndex::FATHER] = makeProcess(currentProcess->id);

        if (hooks.onStart)
        {
            hooks.onStart(instance);
        }

        // Push ID do processo criado
        PUSH(makeInt(instance->id));

        //  Não criou frame no current fiber!
        DISPATCH();
    }

    // ========================================
    // PATH 4: STRUCT
    // ========================================
    else if (callee.isStruct())
    {
        int index = callee.as.integer;
        StructDef *def = structs[index];

        if (argCount != def->argCount)
        {
            runtimeError("Struct '%s' expects %zu arguments, got %d",
                         def->name->chars(), def->argCount, argCount);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Value value = makeStructInstance();
        StructInstance *instance = value.as.sInstance;
        instance->def = def;
        structInstances.push(instance);
        instance->values.reserve(argCount);

        for (int i = argCount - 1; i >= 0; i--)
        {
            instance->values[i] = POP();
        }
        POP(); // Remove callee
        PUSH(value);

        DISPATCH();
    }

    // ========================================
    // PATH 5: CLASS
    // ========================================
    else if (callee.isClass())
    {
        int classId = callee.asClassId();
        ClassDef *klass = classes[classId];

        Value value = makeClassInstance();
        ClassInstance *instance = value.asClassInstance();
        instance->klass = klass;
        instance->fields.reserve(klass->fieldCount);

        // Inicializa fields com nil
        for (int i = 0; i < klass->fieldCount; i++)
        {
            instance->fields.push(makeNil());
        }

        // Substitui class por instance na stack
        fiber->stackTop[-argCount - 1] = value;

        if (klass->constructor)
        {
            if (argCount != klass->constructor->arity)
            {
                runtimeError("init() expects %d arguments, got %d",
                             klass->constructor->arity, argCount);
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            if (currentFiber->frameCount >= FRAMES_MAX)
            {
                runtimeError("Stack overflow");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            CallFrame *newFrame = &currentFiber->frames[currentFiber->frameCount++];
            newFrame->func = klass->constructor;
            newFrame->ip = klass->constructor->chunk->code;
            newFrame->slots = currentFiber->stackTop - argCount - 1;

            //  Criou frame! Reload!
            LOAD_FRAME();
            DISPATCH();
        }
        else
        {
            // Sem constructor - só remove args
            fiber->stackTop -= argCount;

            //  Não criou frame!
            DISPATCH();
        }
    }

    // ========================================
    // PATH 6: NATIVE CLASS
    // ========================================
    else if (callee.isNativeClass())
    {
        int classId = callee.asClassNativeId();
        NativeClassDef *klass = nativeClasses[classId];

        if (argCount != klass->argCount)
        {
            runtimeError("Native class expects %d args, got %d",
                         klass->argCount, argCount);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Value *args = fiber->stackTop - argCount;
        void *userData = klass->constructor(this, argCount, args);

        if (!userData)
        {
            runtimeError("Failed to create native '%s' instance",
                         klass->name->chars());
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Value literal = makeNativeClassInstance();
        NativeClassInstance *instance = literal.as.sClassInstance;
  
        instance->klass = klass;
        instance->userData = userData;

        // Remove args + callee, push instance
        fiber->stackTop -= (argCount + 1);
        PUSH(literal);

        //  Não criou frame!
        DISPATCH();
    }

    // ========================================
    // PATH 7: NATIVE STRUCT
    // ========================================
    else if (callee.isNativeStruct())
    {
        int structId = callee.asNativeStructId();
        NativeStructDef *def = nativeStructs[structId];

        void *data = arena.Allocate(def->structSize);
        std::memset(data, 0, def->structSize);

        if (def->constructor)
        {
            Value *args = fiber->stackTop - argCount;
            def->constructor(this, data, argCount, args);
        }

        Value literal = makeNativeStructInstance();
        NativeStructInstance *instance = literal.as.sNativeStruct;
        nativeStructInstances.push(instance);
        instance->def = def;
        instance->data = data;

        // Remove args + callee, push instance
        fiber->stackTop -= (argCount + 1);
        PUSH(literal);

        //  Não criou frame!
        DISPATCH();
    }

    // ========================================
    // PATH 8: MODULE REF
    // ========================================
    else if (callee.isModuleRef())
    {
        uint16 moduleId = (callee.as.unsignedInteger >> 16) & 0xFFFF;
        uint16 funcId = callee.as.unsignedInteger & 0xFFFF;

        if (moduleId >= modules.size())
        {
            runtimeError("Invalid module ID: %d", moduleId);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        ModuleDef *mod = modules[moduleId];
        if (funcId >= mod->functions.size())
        {
            runtimeError("Invalid function ID %d in module '%s'",
                         funcId, mod->name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        NativeFunctionDef &func = mod->functions[funcId];

        if (func.arity != -1 && func.arity != argCount)
        {
            String *funcName;
            mod->getFunctionName(funcId, &funcName);
            runtimeError("Module '%s' expects %d args on function '%s' got %d",
                         mod->name->chars(), func.arity,
                         funcName->chars(), argCount);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Value result = func.ptr(this, argCount, fiber->stackTop - argCount);
        fiber->stackTop -= (argCount + 1);
        PUSH(result);

        //  Não criou frame!
        DISPATCH();
    }

    // ========================================
    // ERRO: Tipo desconhecido
    // ========================================
    else
    {
        runtimeError("Can only call functions");
        printf("> ");
        printValue(callee);
        printf("\n");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
}

op_return:
{

    // printf("[DEBUG] OP_RETURN - frameCount: %d\n", fiber->frameCount);
    // printf("[DEBUG] IP offset: %ld\n", (long)(ip - func->chunk->code));

    Value result = POP();

    // printf("[DEBUG] Popped value type: %d\n", (int)result.type);

    fiber->frameCount--;

    if (fiber->frameCount == 0)
    {
        fiber->stackTop = fiber->stack;
        *fiber->stackTop++ = result;

        fiber->state = FiberState::DEAD;

        if (fiber == &currentProcess->fibers[0])
        {
            for (int i = 0; i < currentProcess->nextFiberIndex; i++)
            {
                currentProcess->fibers[i].state = FiberState::DEAD;
            }
            currentProcess->state = FiberState::DEAD;
        }

        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    //  Função nested - retorna para onde estava a chamada
    CallFrame *finished = &fiber->frames[fiber->frameCount];

    fiber->stackTop = finished->slots;
    *fiber->stackTop++ = result;

    LOAD_FRAME();
    DISPATCH();
    // printf("end");
}

    // ========== PROCESS/FIBER CONTROL ==========

op_yield:
{
    Value value = POP();
    float ms = value.isInt()
                   ? (float)value.asInt()
                   : (float)value.asDouble();

    STORE_FRAME();
    return {FiberResult::FIBER_YIELD, instructionsRun, ms, 0};
}

op_frame:
{
    Value value = POP();
    int percent = value.isInt() ? value.asInt() : (int)value.asDouble();

    STORE_FRAME();
    return {FiberResult::PROCESS_FRAME, instructionsRun, 0, percent};
}

op_exit:
{
    Value exitCode = POP();

    // Define exit code (int ou 0)
    currentProcess->exitCode = exitCode.isInt() ? exitCode.asInt() : 0;

    // Mata o processo
    currentProcess->state = FiberState::DEAD;

    // Mata todas as fibers (incluindo a atual)
    for (int i = 0; i < MAX_FIBERS; i++)
    {
        Fiber *f = &currentProcess->fibers[i];
        f->state = FiberState::DEAD;
        f->frameCount = 0;
        f->ip = nullptr;
        f->stackTop = f->stack; // stack limpa
    }

    // (Opcional) deixa o exitCode no topo da fiber atual para debug
    fiber->stackTop = fiber->stack;
    *fiber->stackTop++ = exitCode;

    STORE_FRAME();
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

op_spawn:
{
    uint8 argCount = READ_BYTE();
    Value callee = NPEEK(argCount);

    if (!currentProcess)
    {
        runtimeError("No current process for spawn");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (currentProcess->nextFiberIndex >= MAX_FIBERS)
    {
        runtimeError("Too many fibers in process (max %d)", MAX_FIBERS);
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (!callee.isFunction())
    {
        runtimeError("fiber expects a function");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    int funcIndex = callee.asFunctionId();
    Function *func = functions[funcIndex];

    if (!func)
    {
        runtimeError("Invalid function");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (argCount != func->arity)
    {
        runtimeError("Expected %d arguments but got %d", func->arity, argCount);
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    int fiberIdx = currentProcess->nextFiberIndex++;
    Fiber *newFiber = &currentProcess->fibers[fiberIdx];

    newFiber->state = FiberState::RUNNING;
    newFiber->resumeTime = 0;
    newFiber->stackTop = newFiber->stack;
    newFiber->frameCount = 0;

    for (int i = 0; i < argCount; i++)
    {
        newFiber->stack[i] = fiber->stackTop[-(argCount - i)];
    }
    newFiber->stackTop = newFiber->stack + argCount;

    CallFrame *frame = &newFiber->frames[newFiber->frameCount++];
    frame->func = func;
    frame->ip = func->chunk->code;
    frame->slots = newFiber->stack; // Argumentos começam aqui

    fiber->stackTop -= (argCount + 1);

    PUSH(makeInt(fiberIdx));

    DISPATCH();
}

    // ========== DEBUG ==========

op_print:
{
    Value value = POP();
    printValue(value);
    printf("\n");
    DISPATCH();
}

op_len:
{
    Value value = PEEK();
    if (value.isString())
    {
        DROP();
        PUSH(makeInt(value.asString()->length()));
    }
    else if (value.isArray())
    {
        DROP();
        PUSH(makeInt(value.asArray()->values.size()));
    }
    else if (value.isMap())
    {
        DROP();
        PUSH(makeInt(value.asMap()->table.count));
    }
    else
    {
        runtimeError("len() expects (string , array , map)");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }
    DISPATCH();
}

    // ========== PROPERTY ACCESS ==========

op_get_property:
{
    Value object = PEEK();
    Value nameValue = READ_CONSTANT();

    // printf("\nGet Object: '");
    // printValue(object);
    // printf("'\nName : '");
    // printValue(nameValue);
    // printf("'\n");

    if (!nameValue.isString())
    {
        runtimeError("Property name must be string");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    const char *name = nameValue.asStringChars();

    // === STRING METHODS ===

    if (object.isString())
    {

        if (compare_strings(nameValue.asString(), staticNames[STATIC_LENGTH]))
        {

            DROP();
            PUSH(makeInt(object.asString()->length()));
        }
        else
        {

            runtimeError("String has no property '%s'", name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
    }

    // === PROCESS PRIVATES (external access) ===
    if (object.isProcess())
    {

        int processId = object.asProcessId();

        Process *proc = aliveProcesses[processId];
        if (!proc)
        {
            runtimeError("Process '%i' is dead or invalid", processId);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        int privateIdx = getProcessPrivateIndex(name);
        if (privateIdx != -1)
        {
            PUSH(proc->privates[privateIdx]);
        }
        else
        {
            runtimeError("Proces  does not support '%s' property access", name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        DISPATCH();
    }

    if (object.isStructInstance())
    {

        StructInstance *inst = object.asStructInstance();
        if (!inst)
        {
            runtimeError("Struct is null");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        uint8 value = 0;
        if (inst->def->names.get(nameValue.asString(), &value))
        {

            DROP();
            PUSH(inst->values[value]);
        }
        else
        {
            runtimeError("Struct '%s' has no field '%s'", inst->def->name->chars(), name);
            PUSH(makeNil());
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        DISPATCH();
    }

    if (object.isClassInstance())
    {
        ClassInstance *instance = object.asClassInstance();

        // bool inherited = instance->klass->inherited;

        uint8_t fieldIdx;
        if (instance->klass->fieldNames.get(nameValue.asString(), &fieldIdx))
        {
            DROP();
            PUSH(instance->fields[fieldIdx]);
            DISPATCH();
        }
        runtimeError("Undefined property '%s'", name);
        PUSH(makeNil());
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (object.isNativeClassInstance())
    {

        NativeClassInstance *instance = object.asNativeClassInstance();
        NativeClassDef *klass = instance->klass;
        NativeProperty prop;
        if (instance->klass->properties.get(nameValue.asString(), &prop))
        {
            DROP(); // Remove object

            //  Chama getter
            Value result = prop.getter(this, instance->userData);
            PUSH(result);
            DISPATCH();
        }

        runtimeError("Undefined property '%s' on native class '%s", nameValue.asStringChars(), klass->name->chars());
        DROP();
        PUSH(makeNil());
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (object.isNativeStructInstance())
    {

        NativeStructInstance *inst = object.asNativeStructInstance();

        NativeStructDef *def = inst->def;

        NativeFieldDef field;

        if (!def->fields.get(nameValue.asString(), &field))
        {
            runtimeError("Undefined field '%s' on native struct '%s", nameValue.asStringChars(), def->name->chars());
            DROP();
            PUSH(makeNil());
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        char *base = (char *)inst->data;
        char *ptr = base + field.offset;

        Value result;
        switch (field.type)
        {
        case FieldType::BYTE:
        {
            result = makeByte(*(uint8 *)ptr);
            DISPATCH();
        }
        case FieldType::INT:
            result = makeInt(*(int *)ptr);
            DISPATCH();

        case FieldType::UINT:
            result = makeUInt(*(uint32 *)ptr);
            DISPATCH();

        case FieldType::FLOAT:
            result = makeFloat(*(float *)ptr);
            DISPATCH();
        case FieldType::DOUBLE:
            result = makeDouble(*(double *)ptr);
            DISPATCH();

        case FieldType::BOOL:
            result = makeBool(*(bool *)ptr);
            DISPATCH();

        case FieldType::POINTER:
            result = makePointer(*(void **)ptr);
            DISPATCH();

        case FieldType::STRING:
        {
            String *str = *(String **)ptr;
            result = str ? makeString(str) : makeNil();
            DISPATCH();
        }
        }

        DROP(); // Remove object
        PUSH(result);
        DISPATCH();
    }

    runtimeError("Type does not support 'set' property access");

    printf("[Object: '");
    printValue(object);
    printf("' Property : '");
    printValue(nameValue);

    printf("']\n");

    PUSH(makeNil());
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}
op_set_property:
{
    // Stack: [object, value]
    Value value = PEEK();
    Value object = PEEK2();
    Value nameValue = READ_CONSTANT();

    // printf("Set Value: '");
    // printValue(value);
    // printf("'\nObject: '");
    // printValue(object);
    // printf("'\nName : '");
    // printValue(nameValue);
    // printf("'\n");

    if (!nameValue.isString())
    {
        runtimeError("Property name must be string");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    String *propName = nameValue.asString();
    const char *name = propName->chars();

    // === STRINGS (read-only) ===
    if (object.isString())
    {
        runtimeError("Cannot set property on string (immutable)");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    // === PROCESS PRIVATES (external write) ===
    if (object.isProcess())
    {
        int processId = object.asProcessId();
        Process *proc = aliveProcesses[processId];

        if (!proc) // || proc->state == FiberState::DEAD)
        {
            runtimeError("Process '%i' is dead or invalid", processId);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // Lookup private pelo nome
        int privateIdx = getProcessPrivateIndex(name);
        if (privateIdx != -1)
        {
            if ((privateIdx == (int)PrivateIndex::ID) || (privateIdx == (int)PrivateIndex::FATHER))
            {
                runtimeError("Property '%s' is readonly", name);
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            proc->privates[privateIdx] = value;
            DROP();      // Remove value
            DROP();      // Remove process
            PUSH(value); // Assignment retorna valor
            DISPATCH();
        }

        runtimeError("Process has no property '%s'", name);
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (object.isStructInstance())
    {

        StructInstance *inst = object.asStructInstance();
        if (!inst)
        {
            runtimeError("Struct is null ");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        uint8 valueIndex = 0;
        if (inst->def->names.get(nameValue.asString(), &valueIndex))
        {
            inst->values[valueIndex] = value;
        }
        else
        {
            runtimeError("Struct '%s' has no field '%s'", inst->def->name->chars(), name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // Pop object, keep value
        DROP(); // object

        DISPATCH();
    }

    if (object.isClassInstance())
    {
        ClassInstance *instance = object.asClassInstance();

        uint8_t fieldIdx;
        if (instance->klass->fieldNames.get(nameValue.asString(), &fieldIdx))
        {
            instance->fields[fieldIdx] = value;
            // Pop object, mantém value
            DROP(); // object
            DISPATCH();
        }

        runtimeError("Undefined property '%s'", name);
        DROP(); // object
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (object.isNativeClassInstance())
    {

        NativeClassInstance *instance = object.asNativeClassInstance();
        NativeClassDef *klass = instance->klass;
        NativeProperty prop;
        if (instance->klass->properties.get(nameValue.asString(), &prop))
        {
            if (!prop.setter)
            {
                runtimeError("Property '%s' from class '%s' is read-only", nameValue.asStringChars(), klass->name->chars());
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            // Chama setter
            prop.setter(this, instance->userData, value);
            DROP(); // Remove object
            DISPATCH();
        }
    }

    if (object.isNativeStructInstance())
    {
        NativeStructInstance *inst = object.asNativeStructInstance();
        NativeStructDef *def = inst->def;

        NativeFieldDef field;
        if (!def->fields.get(nameValue.asString(), &field))
        {
            runtimeError("Undefined field '%s' in struct '%s", nameValue.asStringChars(), def->name->chars());
            DROP();
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // Read-only check
        if (field.readOnly)
        {
            runtimeError("Field '%s' is read-only in struct '%s", nameValue.asStringChars(), def->name->chars());
            DROP();
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        char *base = (char *)inst->data;
        char *ptr = base + field.offset;
        switch (field.type)
        {
        case FieldType::BYTE:
        {
            if (!value.isByte())
            {
                runtimeError("Field expects byte");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(uint8 *)ptr = (uint8)value.asByte();
            DISPATCH();
        }

        case FieldType::INT:
            if (!value.isInt())
            {
                runtimeError("Field expects int");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(int *)ptr = value.asInt();
            DISPATCH();
        case FieldType::UINT:
            if (!value.isUInt())
            {
                runtimeError("Field expects uint");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(uint32 *)ptr = value.asUInt();
            DISPATCH();
        case FieldType::FLOAT:
        {
            if (!value.isFloat())
            {
                runtimeError("Field expects float");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(float *)ptr = value.asFloat();
            DISPATCH();
        }
        case FieldType::DOUBLE:
            if (!value.isDouble())
            {
                runtimeError("Field expects double");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(double *)ptr = value.asDouble();
            DISPATCH();

        case FieldType::BOOL:
            if (!value.isBool())
            {
                runtimeError("Field expects bool");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(bool *)ptr = value.asBool();
            DISPATCH();

        case FieldType::POINTER:
            if (!value.isPointer())
            {
                runtimeError("Field expects pointer");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            *(void **)ptr = value.asPointer();
            DISPATCH();

        case FieldType::STRING:
        {
            if (!value.isString())
            {
                runtimeError("Field expects string");
                DROP();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            String **fieldPtr = (String **)ptr;
            *fieldPtr = value.asString();
            DISPATCH();
        }
        }

        DROP(); // Remove object
        DISPATCH();
    }

    runtimeError("Cannot 'set' property on this type");
    printf("[Object: '");
    printValue(object);
    printf("' Property : '");
    printValue(nameValue);

    printf("']\n");

    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};

    DISPATCH();
}
op_invoke:
{
    Value nameValue = READ_CONSTANT();
    uint8_t argCount = READ_BYTE();

    if (!nameValue.isString())
    {
        runtimeError("Method name must be string");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    const char *name = nameValue.asStringChars();
    Value receiver = NPEEK(argCount);

#define ARGS_CLEANUP() fiber->stackTop -= (argCount + 1)

    // === STRING METHODS ===
    if (receiver.isString())
    {
        String *str = receiver.asString();

        if (compare_strings(nameValue.asString(), staticNames[STATIC_LENGTH]))
        {
            int len = str->length();
            ARGS_CLEANUP();
            PUSH(makeInt(len));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_UPPER]))
        {
            ARGS_CLEANUP();
            PUSH(makeString(stringPool.upper(str)));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_LOWER]))
        {
            ARGS_CLEANUP();
            PUSH(makeString(stringPool.lower(str)));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_CONCAT]))
        {
            if (argCount != 1)
            {
                runtimeError("concat() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value arg = PEEK();
            if (!arg.isString())
            {
                runtimeError("concat() expects string argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            String *result = stringPool.concat(str, arg.asString());
            ARGS_CLEANUP();
            PUSH(makeString(result));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_SUB]))
        {
            if (argCount != 2)
            {
                runtimeError("sub() expects 2 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value start = PEEK2();
            Value end = PEEK();

            if (!start.isNumber() || !end.isNumber())
            {
                runtimeError("sub() expects 2 number arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            String *result = stringPool.substring(
                str,
                (uint32_t)start.asNumber(),
                (uint32_t)end.asNumber());
            ARGS_CLEANUP();
            PUSH(makeString(result));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_REPLACE]))
        {
            if (argCount != 2)
            {
                runtimeError("replace() expects 2 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value oldStr = PEEK2();
            Value newStr = PEEK();

            if (!oldStr.isString() || !newStr.isString())
            {
                runtimeError("replace() expects 2 string arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            String *result = stringPool.replace(
                str,
                oldStr.asStringChars(),
                newStr.asStringChars());
            ARGS_CLEANUP();
            PUSH(makeString(result));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_AT]))
        {
            if (argCount != 1)
            {
                runtimeError("at() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value index = PEEK();
            if (!index.isNumber())
            {
                runtimeError("at() expects number argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            String *result = stringPool.at(str, (int)index.asNumber());
            ARGS_CLEANUP();
            PUSH(makeString(result));
        }

        else if (compare_strings(nameValue.asString(), staticNames[STATIC_CONTAINS]))
        {
            if (argCount != 1)
            {
                runtimeError("contains() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value substr = PEEK();
            if (!substr.isString())
            {
                runtimeError("contains() expects string argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            bool result = stringPool.contains(str, substr.asString());
            ARGS_CLEANUP();
            PUSH(makeBool(result));
        }

        else if (compare_strings(nameValue.asString(), staticNames[STATIC_TRIM]))
        {
            String *result = stringPool.trim(str);
            ARGS_CLEANUP();
            PUSH(makeString(result));
        }

        else if (compare_strings(nameValue.asString(), staticNames[STATIC_STARTWITH]))
        {
            if (argCount != 1)
            {
                runtimeError("startsWith() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value prefix = PEEK();
            if (!prefix.isString())
            {
                runtimeError("startsWith() expects string argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            bool result = stringPool.startsWith(str, prefix.asString());
            ARGS_CLEANUP();
            PUSH(makeBool(result));
        }

        else if (compare_strings(nameValue.asString(), staticNames[STATIC_ENDWITH]))
        {
            if (argCount != 1)
            {
                runtimeError("endsWith() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value suffix = PEEK();
            if (!suffix.isString())
            {
                runtimeError("endsWith() expects string argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            bool result = stringPool.endsWith(str, suffix.asString());
            ARGS_CLEANUP();
            PUSH(makeBool(result));
        }

        else if (compare_strings(nameValue.asString(), staticNames[STATIC_INDEXOF]))
        {
            if (argCount < 1 || argCount > 2)
            {
                runtimeError("indexOf() expects 1 or 2 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value substr;
            int startIndex = 0;

            if (argCount == 1)
            {
                // index_of(substr)
                substr = PEEK();
            }
            else
            {
                // index_of(substr, startIndex)
                Value startVal = PEEK();
                substr = PEEK2();

                if (!startVal.isNumber())
                {
                    runtimeError("indexOf() startIndex must be number");
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }

                startIndex = (int)startVal.asNumber();
            }

            if (!substr.isString())
            {
                runtimeError("indexOf() expects string argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            int result = stringPool.indexOf(
                str,
                substr.asString(),
                startIndex);
            ARGS_CLEANUP();
            PUSH(makeInt(result));
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_REPEAT]))
        {
            if (argCount != 1)
            {
                runtimeError("repeat() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value count = PEEK();
            if (!count.isNumber())
            {
                runtimeError("repeat() expects number argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            String *result = stringPool.repeat(str, (int)count.asNumber());
            ARGS_CLEANUP();
            PUSH(makeString(result));
        }
        else
        {
            runtimeError("String has no method '%s'", name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        DISPATCH();
    }

    // === ARRAY METHODS ===
    if (receiver.isArray())
    {
        ArrayInstance *arr = receiver.asArray();
        uint32 size = arr->values.size();
        if (compare_strings(nameValue.asString(), staticNames[STATIC_PUSH]))
        {
            if (argCount != 1)
            {
                runtimeError("push() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            Value item = PEEK();
            arr->values.push(item);

            ARGS_CLEANUP();

            PUSH(receiver);
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_POP]))
        {
            if (argCount != 0)
            {
                runtimeError("pop() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            if (size == 0)
            {
                Warning("Cannot pop from empty array");
                ARGS_CLEANUP();
                PUSH(receiver);
                DISPATCH();
            }
            else
            {
                Value result = arr->values.back();
                arr->values.pop();
                ARGS_CLEANUP();
                PUSH(result);
            }
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_BACK]))
        {
            if (argCount != 0)
            {
                runtimeError("back() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            if (size == 0)
            {
                Warning("Cannot get back from empty array");
                ARGS_CLEANUP();
                PUSH(receiver);
                DISPATCH();
            }
            else
            {
                Value result = arr->values.back();
                ARGS_CLEANUP();
                PUSH(result);
            }
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_LENGTH]))
        {
            if (argCount != 0)
            {
                runtimeError("lenght() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            ARGS_CLEANUP();
            PUSH(makeInt(size));
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_CLEAR]))
        {
            if (argCount != 0)
            {
                runtimeError("clear() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            arr->values.clear();
            ARGS_CLEANUP();
            PUSH(receiver);
            DISPATCH();
        }
        else
        {
            runtimeError("Array has no method '%s'", name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
    }

    // === MAP METHODS ===
    if (receiver.isMap())
    {
        MapInstance *map = receiver.asMap();

        if (compare_strings(nameValue.asString(), staticNames[STATIC_HAS]))
        {
            if (argCount != 1)
            {
                runtimeError("has() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value key = PEEK();

            if (!key.isString())
            {
                runtimeError("Map key must be string");
                ARGS_CLEANUP();
                PUSH(makeBool(false));
                DISPATCH();
            }

            bool exists = map->table.exist(key.asString());
            ARGS_CLEANUP();
            PUSH(makeBool(exists));
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_REMOVE]))
        {
            if (argCount != 1)
            {
                runtimeError("remove() expects 1 argument");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            Value key = PEEK();

            if (!key.isString())
            {
                runtimeError("Map key must be string");
                ARGS_CLEANUP();
                PUSH(makeNil());
                DISPATCH();
            }

            //  HashMap não tem remove, mas podes setar para nil
            map->table.set(key.asString(), makeNil());
            ARGS_CLEANUP();
            PUSH(makeNil());
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_CLEAR]))
        {
            if (argCount != 0)
            {
                runtimeError("clear() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            map->table.destroy();
            ARGS_CLEANUP();
            PUSH(makeNil());
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_LENGTH]))
        {
            if (argCount != 0)
            {
                runtimeError("lenght() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            ARGS_CLEANUP();
            PUSH(makeInt(map->table.count));
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_KEYS]))
        {
            if (argCount != 0)
            {
                runtimeError("keys() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            // Retorna array de keys
            Value keys = makeArray();
            ArrayInstance *keysInstance = keys.asArray();

            map->table.forEach([&](String *key, Value value)
                               { keysInstance->values.push(makeString(key)); });

            ARGS_CLEANUP();
            PUSH(keys);
            DISPATCH();
        }
        else if (compare_strings(nameValue.asString(), staticNames[STATIC_VALUES]))
        {
            if (argCount != 0)
            {
                runtimeError("values() expects 0 arguments");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            // Retorna array de values
            Value values = makeArray();
            ArrayInstance *valueInstance = values.asArray();

            map->table.forEach([&](String *key, Value value)
                               { valueInstance->values.push(value); });

            ARGS_CLEANUP();
            PUSH(values);
            DISPATCH();
        }
    }

    // === CLASS INSTANCE METHODS ===
    if (receiver.isClassInstance())
    {
        ClassInstance *instance = receiver.asClassInstance();
        // printValueNl(receiver);
        // printValueNl(nameValue);

        Function *method;
        if (instance->getMethod(nameValue.asString(), &method))
        {
            if (argCount != method->arity)
            {
                runtimeError("Method '%s' expects %d arguments, got %d", name, method->arity, argCount);
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            //  Debug::dumpFunction(method);
            fiber->stackTop[-argCount - 1] = receiver;

            // Setup call frame
            if (currentFiber->frameCount >= FRAMES_MAX)
            {
                runtimeError("Stack overflow in method!");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            CallFrame *newFrame = &currentFiber->frames[currentFiber->frameCount];
            newFrame->func = method;
            newFrame->ip = method->chunk->code;
            newFrame->slots = currentFiber->stackTop - argCount - 1;

            currentFiber->frameCount++;

            STORE_FRAME();
            LOAD_FRAME();

            DISPATCH();
        }
        runtimeError("Instance '%s' has no method '%s'", instance->klass->name->chars(), name);
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (receiver.isNativeClassInstance())
    {
        printValueNl(receiver);
        printValueNl(nameValue);

        NativeClassInstance *instance = receiver.asNativeClassInstance();
        NativeClassDef *klass = instance->klass;

        NativeMethod method;
        if (!instance->klass->methods.get(nameValue.asString(), &method))
        {
            runtimeError("Native class '%s' has no method '%s'", klass->name->chars(), name);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        Value *args = fiber->stackTop - argCount;
        Value result = method(this, instance->userData, argCount, args);
        fiber->stackTop -= (argCount + 1);
        PUSH(result);

        DISPATCH();
    }

    runtimeError("Type does not support method calls");

    printf(": ");
    printValue(receiver);
    printf("\n");

    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

op_super_invoke:
{
    uint8_t ownerClassId = READ_BYTE();
    uint8_t nameIdx = READ_BYTE();
    uint8_t argCount = READ_BYTE();

    Value nameValue = func->chunk->constants[nameIdx];
    String *methodName = nameValue.asString();
    Value self = NPEEK(argCount);

    if (!self.isClassInstance())
    {
        runtimeError("'super' requires an instance");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    ClassInstance *instance = self.asClassInstance();
    ClassDef *ownerClass = classes[ownerClassId];

    // printf("[RUNTIME] super.%s: ownerClassId=%d (%s), super=%s\n",
    //        methodName->chars(), ownerClassId,
    //        ownerClass->name->chars(),
    //        ownerClass->superclass ? ownerClass->superclass->name->chars() : "NULL");

    if (!ownerClass->superclass)
    {
        runtimeError("Class has no superclass");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    Function *method;

    if (compareString(methodName, staticNames[STATIC_INIT]))
    {
        method = ownerClass->superclass->constructor; // ← USA ownerClass!
        if (!method)
        {
            runtimeError("Superclass has no init()");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
    }
    else
    {
        // Métodos normais
        if (!ownerClass->superclass->methods.get(methodName, &method))
        {
            runtimeError("Undefined method '%s'", methodName->chars());
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
    }

    if (argCount != method->arity)
    {
        runtimeError("Method expects %d arguments, got %d",
                     method->arity, argCount);
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    if (fiber->frameCount >= FRAMES_MAX)
    {
        runtimeError("Stack overflow");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    CallFrame *newFrame = &fiber->frames[fiber->frameCount];
    newFrame->func = method;
    newFrame->ip = method->chunk->code;
    newFrame->slots = fiber->stackTop - argCount - 1;
    fiber->frameCount++;

    STORE_FRAME();
    LOAD_FRAME();
    DISPATCH();
}

op_gosub:
{
    int16 off = (int16)READ_SHORT(); // lê u16 mas cast para signed
    if (fiber->gosubTop >= GOSUB_MAX)
        runtimeError("gosub stack overflow");
    fiber->gosubStack[fiber->gosubTop++] = ip; // retorno
    ip += off;                                 // forward/back
    DISPATCH();
}

op_return_sub:
{
    if (fiber->gosubTop > 0)
    {
        ip = fiber->gosubStack[--fiber->gosubTop];
        DISPATCH();
    }
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

op_define_array:
{

    uint8_t count = READ_BYTE();
    Value array = makeArray();
    ArrayInstance *instance = array.asArray();
    instance->values.resize(count);
    for (int i = count - 1; i >= 0; i--)
    {
        instance->values[i] = POP();
    }
    PUSH(array);
    DISPATCH();
}
op_define_map:
{
    uint8_t count = READ_BYTE();

    Value map = makeMap();
    MapInstance *inst = map.asMap();

    for (int i = 0; i < count; i++)
    {
        Value value = POP();
        Value key = POP();

        if (!key.isString())
        {
            runtimeError("Map key must be string");
            PUSH(makeNil());
            DISPATCH();
        }

        inst->table.set(key.asString(), value);
    }

    PUSH(map);
    DISPATCH();
}
op_set_index:
{
    Value value = POP();
    Value index = POP();
    Value container = POP();

    // printValue(value);
    // printf(" value \n");
    // printValue(index);
    // printf(" index \n");
    // printValue(container);
    // printf(" container \n");

    if (container.isArray())
    {
        if (!index.isInt())
        {
            runtimeError("Array index must be integer");
            PUSH(value);
            DISPATCH();
        }

        ArrayInstance *arr = container.asArray();
        int i = index.asInt();
        uint32 size = arr->values.size();

        // Negative index
        if (i < 0)
            i += size;

        if (i < 0 || i >= size)
        {
            runtimeError("Array index %d out of bounds (size=%d)", i, size);
        }
        else
        {
            arr->values[i] = value;
        }

        PUSH(value); // Assignment returns value
        DISPATCH();
    }

    // === MAP  ===
    if (container.isMap())
    {
        if (!index.isString())
        {
            runtimeError("Map key must be string");
            PUSH(value);
            DISPATCH();
        }

        MapInstance *map = container.asMap();
        map->table.set(index.asString(), value);

        PUSH(value); // Assignment returns value
        DISPATCH();
    }

    // STRING é imutável!
    if (container.isString())
    {
        runtimeError("Strings are immutable");
        PUSH(value);
        DISPATCH();
    }

    runtimeError("Cannot index assign this type");
    PUSH(value);
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    DISPATCH();
}
op_get_index:
{
    Value index = POP();
    Value container = POP();

    // printValue(index);
    // printf("\n");
    // printValue(container);
    // printf("\n");

    // === ARRAY ===
    if (container.isArray())
    {
        if (!index.isInt())
        {
            runtimeError("Array index must be integer");
            PUSH(makeNil());
            DISPATCH();
        }

        ArrayInstance *arr = container.asArray();
        int i = index.asInt();
        uint32 size = arr->values.size();

        //  Negative index (Python-style)
        if (i < 0)
            i += size;

        if (i < 0 || i >= size)
        {
            runtimeError("Array index %d out of bounds (size=%d)", i, size);
            PUSH(makeNil());
        }
        else
        {
            PUSH(arr->values[i]);
        }
        DISPATCH();
    }

    // === STRING ===
    if (container.isString())
    {
        if (!index.isInt())
        {
            runtimeError("String index must be integer");
            PUSH(makeNil());
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        String *str = container.asString();
        String *result = stringPool.at(str, index.asInt());
        PUSH(makeString(result));
        DISPATCH();
    }

    // === MAP   ===
    if (container.isMap())
    {
        if (!index.isString())
        {
            runtimeError("Map key must be string");
            PUSH(makeNil());
            DISPATCH();
        }

        MapInstance *map = container.asMap();
        Value result;

        if (map->table.get(index.asString(), &result))
        {
            PUSH(result);
        }
        else
        {
            // Key não existe - retorna nil
            PUSH(makeNil());
        }
        DISPATCH();
    }

    runtimeError("Cannot index this type");
    PUSH(makeNil());
    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
}

op_foreach_start:
{
    Value arr = PEEK(); // [array]

    // printValueNl(arr);

    if (!arr.isArray())
    {
        runtimeError("foreach requires an array");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    PUSH(makeInt(0)); // [array, 0]
    DISPATCH();
}

op_foreach_next:
{
    Value vindex = POP();  // → [array]
    Value varray = PEEK(); // → [array] (não remove)

    if (!varray.isArray())
    {
        runtimeError("foreach requires an array");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    ArrayInstance *array = varray.asArray();
    int index = vindex.asInt();

    if (index >= array->values.size())
    {
        runtimeError("Index out of bounds");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    Value item = array->values[index];

    PUSH(makeInt(index + 1)); // → [array, index+1]
    PUSH(item);                      // → [array, index+1, item]
    DISPATCH();
}
op_foreach_check:
{
    Value index = PEEK();
    Value array = PEEK2();

    //  printf("index: ");
    //  printValue(index);

    // printf("\narray: ");
    // printValueNl(array);

    if (!array.isArray())
    {
        runtimeError("foreach requires an array");
        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
    }

    bool done = index.asInt() < array.asArray()->values.size();

    push(makeBool(done));
    DISPATCH();
}

// Cleanup macros

#undef READ_BYTE
#undef READ_SHORT
}

#endif // USE_COMPUTED_GOTO