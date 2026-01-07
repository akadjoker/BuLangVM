#include "interpreter.hpp"
#include "pool.hpp"
#include "opcode.hpp"
#include "debug.hpp"
#include <cmath> // std::fmod
#include <new>

#ifndef USE_COMPUTED_GOTO

#define DEBUG_TRACE_EXECUTION 0 // 1 = ativa, 0 = desativa
#define DEBUG_TRACE_STACK 0     // 1 = mostra stack, 0 = esconde

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

#define DROP() (fiber->stackTop--)
#define PEEK() (*(fiber->stackTop - 1))
#define PEEK2() (*(fiber->stackTop - 2))

#define POP() (*(--fiber->stackTop))
#define PUSH(value) (*fiber->stackTop++ = value)
#define NPEEK(n) (fiber->stackTop[-1 - (n)])

    // if (frame->ip == nullptr)
    // {
    //     runtimeError("FATAL: frame->ip is NULL! Code was not initialized properly.");
    //     return {FiberResult::FIBER_DONE, 0, 0, 0};
    // }

#define READ_BYTE() (*ip++)
#define READ_SHORT() (ip += 2, (uint16)((ip[-2] << 8) | ip[-1]))

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

#define READ_CONSTANT() (func->chunk->constants[READ_BYTE()])
    LOAD_FRAME();

    // printf("[DEBUG] Starting run_fiber: ip=%p, func=%s, offset=%ld\n",
    //        (void*)ip, func->name->chars(), ip - func->chunk->code);

    // ===== LOOP PRINCIPAL =====

    for (;;)
    {

#if DEBUG_TRACE_STACK
        // Mostra stack
        printf("          ");
        for (Value *slot = fiber->stack; slot < fiber->stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
#endif

#if DEBUG_TRACE_EXECUTION
        // Mostra instrução
        size_t offset = ip - func->chunk->code;
        Debug::disassembleInstruction(func->chunk, offset);
#endif

        //    printf("[EXEC] opcode: %d at offset %ld\n", *ip, (long)(ip - func->chunk->code));

        uint8 instruction = READ_BYTE();

        // if (instruction > 57)
        // {  // Opcode inválido
        //     printf("[ERROR] Invalid opcode %d!\n", instruction);
        //     printf("  func: %s\n", func->name->chars());
        //     printf("  ip offset: %ld\n", (long)(ip - func->chunk->code - 1));
        //     printf("  chunk size: %d\n", func->chunk->count);

        //     // Printa últimos 10 opcodes
        //     printf("  Last 10 opcodes:\n");
        //     for (int i = -10; i < 0; i++) {
        //         if (ip + i >= func->chunk->code) {
        //             printf("    [%d]: %d\n", i, ip[i]);
        //         }
        //     }

        //     runtimeError("Unknown opcode %d", instruction);
        //     return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        // }

        switch (instruction)
        {
            // ========== CONSTANTS ==========

        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            PUSH(constant);
            break;
        }

        case OP_NIL:
            PUSH(makeNil());
            break;
        case OP_TRUE:
            PUSH(makeBool(true));
            break;
        case OP_FALSE:
            PUSH(makeBool(false));
            break;

        case OP_DUP:
        {
            Value top = PEEK();
            PUSH(top);
            break;
        }

        case OP_HALT:
        {
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
            // ========== STACK MANIPULATION ==========

        case OP_POP:
        {
            DROP();

            break;
        }

            // ========== VARIABLES ==========

        case OP_GET_LOCAL:
        {
            uint8 slot = READ_BYTE();
            const Value &value = stackStart[slot];

            // printf("[OP_GET_LOCAL] slot=%d, value=", slot);
            // printValueNl(value); // ← ADICIONA ISTO

            PUSH(value);
            break;
        }

        case OP_SET_LOCAL:
        {
            uint8 slot = READ_BYTE();
            stackStart[slot] = PEEK();
            break;
        }

        case OP_GET_PRIVATE:
        {
            uint8 index = READ_BYTE();
            PUSH(currentProcess->privates[index]);
            break;
        }

        case OP_SET_PRIVATE:
        {
            uint8 index = READ_BYTE();
            currentProcess->privates[index] = PEEK();
            break;
        }

        case OP_GET_GLOBAL:
        {

            Value name = READ_CONSTANT();
            Value value;

            if (!globals.get(name.asString(), &value))
            {
                runtimeError("Undefined variable '%s'", name.asString()->chars());
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(value);
            break;
        }

        case OP_SET_GLOBAL:
        {

            Value name = READ_CONSTANT();
            Value value = PEEK();
            if (globals.set(name.asString(), value))
            {
            }
            break;
        }

        case OP_DEFINE_GLOBAL:
        {

            Value name = READ_CONSTANT();
            globals.set(name.asString(), POP());
            break;
        }

            // ========== ARITHMETIC ==========

        // ============================================
        // OP_ADD
        // ============================================
        case OP_ADD:
        {
            BINARY_OP_PREP();

            // Info(" [OP_ADD] '%s' + '%s'",typeToString(a.type),"+",typeToString(b.type));
            //  String concatenation
            if (a.isString() && b.isString())
            {
                String *result = stringPool.concat(a.asString(), b.asString());
                PUSH(makeString(result));
                break;
            }
            else if (a.isString() && b.isDouble())
            {
                String *right = stringPool.toString(b.asDouble());
                String *result = stringPool.concat(a.asString(), right);
                PUSH(makeString(result));
                break;
            }
            else if (a.isString() && b.isInt())
            {
                String *right = stringPool.toString(b.asInt());
                String *result = stringPool.concat(a.asString(), right);
                PUSH(makeString(result));
                break;
            }
            else if (a.isString() && b.isBool())
            {
                String *right = stringPool.create(b.asBool() ? "true" : "false");
                String *result = stringPool.concat(a.asString(), right);
                PUSH(makeString(result));
                break;
            }
            else if (a.isString() && b.isNil())
            {
                String *right = stringPool.create("nil");
                String *result = stringPool.concat(a.asString(), right);
                PUSH(makeString(result));
                break;
            }
            else
                // Numeric operations
                if (a.isInt() && b.isInt())
                {
                    PUSH(makeInt(a.asInt() + b.asInt()));
                    break;
                }
                else if (a.isInt() && b.isDouble())
                {
                    PUSH(makeDouble(a.asInt() + b.asDouble()));
                    break;
                }
                else if (a.isDouble() && b.isInt())
                {
                    PUSH(makeDouble(a.asDouble() + b.asInt()));
                    break;
                }
                else if (a.isDouble() && b.isDouble())
                {
                    PUSH(makeDouble(a.asDouble() + b.asDouble()));
                    break;
                }

            runtimeError("Operands '+' must be numbers or strings");
            printValueNl(a);
            printValueNl(b);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // ============================================
        // OP_SUBTRACT
        // ============================================
        case OP_SUBTRACT:
        {
            BINARY_OP_PREP();

            if (a.isInt() && b.isInt())
            {
                PUSH(makeInt(a.asInt() - b.asInt()));
                break;
            }
            if (a.isInt() && b.isDouble())
            {
                PUSH(makeDouble(a.asInt() - b.asDouble())); // ← FIX!
                break;
            }
            if (a.isDouble() && b.isInt())
            {
                PUSH(makeDouble(a.asDouble() - b.asInt()));
                break;
            }
            if (a.isDouble() && b.isDouble())
            {
                PUSH(makeDouble(a.asDouble() - b.asDouble()));
                break;
            }

            runtimeError("Operands '-' must be numbers");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // ============================================
        // OP_MULTIPLY
        // ============================================
        case OP_MULTIPLY:
        {
            BINARY_OP_PREP();

            if (a.isInt() && b.isInt())
            {
                PUSH(makeInt(a.asInt() * b.asInt()));
                break;
            }
            if (a.isInt() && b.isDouble())
            {
                PUSH(makeDouble(a.asInt() * b.asDouble())); // ← FIX!
                break;
            }
            if (a.isDouble() && b.isInt())
            {
                PUSH(makeDouble(a.asDouble() * b.asInt()));
                break;
            }
            if (a.isDouble() && b.isDouble())
            {
                PUSH(makeDouble(a.asDouble() * b.asDouble()));
                break;
            }

            runtimeError("Operands '*' must be numbers");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // ============================================
        // OP_DIVIDE
        // ============================================
        case OP_DIVIDE:
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
                break;
            }
            if (a.isInt() && b.isDouble())
            {
                if (b.asDouble() == 0.0)
                {
                    runtimeError("Division by zero");
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }
                PUSH(makeDouble(a.asInt() / b.asDouble()));
                break;
            }
            if (a.isDouble() && b.isInt())
            {
                if (b.asInt() == 0)
                {
                    runtimeError("Division by zero");
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }
                PUSH(makeDouble(a.asDouble() / b.asInt()));
                break;
            }
            if (a.isDouble() && b.isDouble())
            {
                if (b.asDouble() == 0.0)
                {
                    runtimeError("Division by zero");
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }
                PUSH(makeDouble(a.asDouble() / b.asDouble()));
                break;
            }

            runtimeError("Operands must be numbers");
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        // ============================================
        // OP_MODULO
        // ============================================
        case OP_MODULO:
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
                break;
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
            break;
        }

            //======== LOGICAL =====

        case OP_NEGATE:
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
            break;
        }

        case OP_EQUAL:
        {
            BINARY_OP_PREP();
            PUSH(makeBool(valuesEqual(a, b)));

            break;
        }

        case OP_NOT:
        {
            Value v = POP();
            PUSH(makeBool(!isTruthy(v)));
            break;
        }

        case OP_NOT_EQUAL:
        {
            BINARY_OP_PREP();
            PUSH(makeBool(!valuesEqual(a, b)));
            break;
        }

        case OP_GREATER:
        {
            BINARY_OP_PREP();

            double da, db;
            if (!toNumberPair(a, b, da, db))
            {
                runtimeError("Operands '>' must be numbers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            PUSH(makeBool(da > db));
            break;
        }

        case OP_GREATER_EQUAL:
        {
            BINARY_OP_PREP();

            double da, db;
            if (!toNumberPair(a, b, da, db))
            {
                runtimeError("Operands '>=' must be numbers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeBool(da >= db));
            break;
        }

        case OP_LESS:
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
            break;
        }

        case OP_LESS_EQUAL:
        {
            BINARY_OP_PREP();
            double da, db;
            if (!toNumberPair(a, b, da, db))
            {
                runtimeError("Operands  '<=' must be numbers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeBool(da <= db));
            break;
        }

            // ======= BITWISE =====

        case OP_BITWISE_AND:
        {
            BINARY_OP_PREP();
            if (!a.isInt() || !b.isInt())
            {
                runtimeError("Bitwise AND requires integers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeInt(a.asInt() & b.asInt()));
            break;
        }

        case OP_BITWISE_OR:
        {
            BINARY_OP_PREP();
            if (!a.isInt() || !b.isInt())
            {
                runtimeError("Bitwise OR requires integers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeInt(a.asInt() | b.asInt()));
            break;
        }

        case OP_BITWISE_XOR:
        {
            BINARY_OP_PREP();
            if (!a.isInt() || !b.isInt())
            {
                runtimeError("Bitwise XOR requires integers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeInt(a.asInt() ^ b.asInt()));
            break;
        }

        case OP_BITWISE_NOT:
        {
            Value a = POP();
            if (!a.isInt())
            {
                runtimeError("Bitwise NOT requires integer");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeInt(~a.asInt()));
            break;
        }

        case OP_SHIFT_LEFT:
        {
            BINARY_OP_PREP();
            if (!a.isInt() || !b.isInt())
            {
                runtimeError("Shift left requires integers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeInt(a.asInt() << b.asInt()));
            break;
        }

        case OP_SHIFT_RIGHT:
        {
            BINARY_OP_PREP();
            if (!a.isInt() || !b.isInt())
            {
                runtimeError("Shift right requires integers");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }
            PUSH(makeInt(a.asInt() >> b.asInt()));
            break;
        }

            // ========== CONTROL FLOW ==========

        case OP_JUMP:
        {
            uint16 offset = READ_SHORT();
            ip += offset;
            break;
        }

        case OP_JUMP_IF_FALSE:
        {
            uint16 offset = READ_SHORT();
            if (isFalsey(PEEK()))
                ip += offset;
            break;
        }

        case OP_LOOP:
        {

            uint16 offset = READ_SHORT();
            ip -= offset;

            break;
        }

            // ========== FUNCTIONS ==========

        case OP_CALL:
        {
            uint8 argCount = READ_BYTE();

            STORE_FRAME();

            Value callee = NPEEK(argCount);

            //  printf("Call : (");
            //   printValue(callee);
            //   printf(") count %d\n", argCount);

            if (callee.isFunction())
            {
                int index = callee.asFunctionId();

                Function *func = functions[index];
                if (!func)
                {
                    runtimeError("Invalid function");
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }

                // Debug::dumpFunction(func);

                if (argCount != func->arity)
                {
                    runtimeError("Function %s expected %d arguments but got %d", func->name->chars(), func->arity, argCount);
                    for (int i = 0; i < argCount; i++)
                    {
                        printValue(NPEEK(i));
                        printf(" ");
                    }
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }

                if (fiber->frameCount >= FRAMES_MAX)
                {
                    runtimeError("Stack overflow");
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }

                CallFrame *newFrame = &fiber->frames[fiber->frameCount++];
                newFrame->func = func;
                newFrame->ip = func->chunk->code;
                newFrame->slots = fiber->stackTop - argCount - 1; // Argumentos começam aqui
            }
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

                // Remove args + callee da stack
                fiber->stackTop -= (argCount + 1);

                // push resultado
                PUSH(result);
            }
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

                // Verifica arity
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
                            uint8 index = blueprint->argsNames[i];
                            if (index != 255)
                            {
                                instance->privates[index] = procFiber->stack[i];
                            }
                        }
                    }

                    procFiber->stackTop = procFiber->stack + argCount;

                    // Os argumentos viram locals[0], locals[1]...
                }

                // Remove callee + args da stack atual
                fiber->stackTop -= (argCount + 1);

                if (currentProcess->id == 0)
                {
                }

                instance->privates[(int)PrivateIndex::ID] = makeInt(instance->id);
                instance->privates[(int)PrivateIndex::FATHER] = makeProcess(currentProcess->id);

                if (hooks.onStart)
                {
                    hooks.onStart(instance);
                }

                // Push ID do processo criado
                PUSH(makeInt(instance->id));
            }
            else if (callee.isStruct())
            {
                int index = callee.as.integer;

                StructDef *def = structs[index];

                if (argCount != def->argCount)
                {
                    runtimeError("Struct '%s' expects %zu arguments, got %d", def->name->chars(), def->argCount, argCount);
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
                POP();
                PUSH(value);
                break;
            }
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
                        runtimeError("init() expects %d arguments, got %d", klass->constructor->arity, argCount);
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }

                    if (currentFiber->frameCount >= FRAMES_MAX)
                    {
                        runtimeError("Stack overflow");
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }

                    CallFrame *newFrame = &currentFiber->frames[currentFiber->frameCount];
                    newFrame->func = klass->constructor;
                    newFrame->ip = klass->constructor->chunk->code;
                    newFrame->slots = currentFiber->stackTop - argCount - 1;

                    currentFiber->frameCount++;

                    STORE_FRAME();
                    LOAD_FRAME();
                }
                else
                {
                    // Sem init - pop args
                    fiber->stackTop -= argCount;
                }

                break;
            }
            else if (callee.isNativeClass())
            {
                int classId = callee.asClassNativeId();
                NativeClassDef *klass = nativeClasses[classId];

                if (argCount != klass->argCount)
                {
                    runtimeError("Native class expects %d args, got %d", klass->argCount, argCount);
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }

                Value *args = fiber->stackTop - argCount;
                void *userData = klass->constructor(this, argCount, args);

                if (!userData)
                {
                    runtimeError("Failed to create native '%s' instance", klass->name->chars());
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }
                Value literal = makeNativeClassInstance();
                // Cria instance wrapper
                NativeClassInstance *instance = literal.as.sClassInstance;

                instance->klass = klass;
                instance->userData = userData;

                // Remove args + callee, push instance
                fiber->stackTop -= (argCount + 1);
                PUSH(literal);

                break;
            }

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
                // Cria instance wrapper
                NativeStructInstance *instance = literal.as.sNativeStruct;
                nativeStructInstances.push(instance);
                instance->def = def;
                instance->data = data;

                // Remove args + callee, push instance
                fiber->stackTop -= (argCount + 1);
                PUSH(literal);

                break;
            }
            else if (callee.isModuleRef())
            {

                uint32 packed = callee.as.unsignedInteger;
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
                    runtimeError("Invalid function ID %d in module '%s'", funcId, mod->name);
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }
                NativeFunctionDef &func = mod->functions[funcId];
                // Info("args count: %d funcId: %d arity: %d", argCount,funcId,func.arity);
                if (func.arity != -1 && func.arity != argCount)
                {
                    String *funcName;
                    mod->getFunctionName(funcId, &funcName);
                    runtimeError("Module '%s' expects %d args on function '%s' got %d", mod->name->chars(), func.arity, funcName->chars(), argCount);
                    return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                }
                Value result = func.ptr(this, argCount, fiber->stackTop - argCount);
                fiber->stackTop -= (argCount + 1);
                PUSH(result);
                break;
            }
            else
            {

                runtimeError("Can only call functions");
                printf("> ");
                printValue(callee);
                printf("\n");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            LOAD_FRAME();
            break;
        }

        case OP_RETURN:
        {

            Value result = POP();

            bool hasFinally = false;
            if (fiber->tryDepth > 0)
            {
                for (int depth = fiber->tryDepth - 1; depth >= 0; depth--)
                {
                    TryHandler &handler = fiber->tryHandlers[depth];

                    if (handler.finallyIP != nullptr && !handler.inFinally)
                    {
                        // Marca para executar finally
                        handler.pendingReturn = result;
                        handler.hasPendingReturn = true;
                        handler.inFinally = true;
                        fiber->tryDepth = depth + 1; // Ajusta depth
                        ip = handler.finallyIP;
                        hasFinally = true;
                        break;
                    }
                }
            }

            // Se tem finally, EXIT_FINALLY vai lidar com o return
            if (hasFinally)
            {
                break;
            }

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

                STORE_FRAME();
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            //  Função nested - retorna para onde estava a chamada
            CallFrame *finished = &fiber->frames[fiber->frameCount];
            fiber->stackTop = finished->slots;
            *fiber->stackTop++ = result;
            LOAD_FRAME();

            break;
        }

            // ========== PROCESS/FIBER CONTROL ==========

        case OP_YIELD:
        {
            Value value = POP();
            float ms = value.isInt()
                           ? (float)value.asInt()
                           : (float)value.asDouble();

            STORE_FRAME();
            return {FiberResult::FIBER_YIELD, instructionsRun, ms, 0};
        }

        case OP_FRAME:
        {
            Value value = POP();
            int percent = value.isInt() ? value.asInt() : (int)value.asDouble();

            STORE_FRAME();
            return {FiberResult::PROCESS_FRAME, instructionsRun, 0, percent};
        }

        case OP_EXIT:
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
        case OP_SPAWN:
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

            break;
        }

            // ========== DEBUG ==========

        case OP_PRINT:
        {
            uint8_t argCount = READ_BYTE();

            // Pop argumentos na ordem reversa (último empilhado = último impresso)
            Value *args = fiber->stackTop - argCount;

            for (uint8_t i = 0; i < argCount; i++)
            {
                printValue(args[i]);
                // if (i < argCount - 1)
                // {
                //     printf(" "); // Espaço entre argumentos
                // }
            }
           printf("\n");

            // Remove argumentos da stack
            fiber->stackTop -= argCount;
            break;
        }

        case OP_FUNC_LEN:
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
            break;
        }

            // ========== PROPERTY ACCESS ==========

        case OP_GET_PROPERTY:
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

                break;
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
                break;
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
                    break;
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
                    break;
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
                    break;
                }
                case FieldType::INT:
                    result = makeInt(*(int *)ptr);
                    break;

                case FieldType::UINT:
                    result = makeUInt(*(uint32 *)ptr);
                    break;

                case FieldType::FLOAT:
                    result = makeFloat(*(float *)ptr);
                    break;
                case FieldType::DOUBLE:
                    result = makeDouble(*(double *)ptr);
                    break;

                case FieldType::BOOL:
                    result = makeBool(*(bool *)ptr);
                    break;

                case FieldType::POINTER:
                    result = makePointer(*(void **)ptr);
                    break;

                case FieldType::STRING:
                {
                    String *str = *(String **)ptr;
                    result = str ? makeString(str) : makeNil();
                    break;
                }
                }

                DROP(); // Remove object
                PUSH(result);
                break;
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
        case OP_SET_PROPERTY:
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
                    break;
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

                break;
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
                    break;
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
                    break;
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
                    break;
                }

                case FieldType::INT:
                    if (!value.isInt())
                    {
                        runtimeError("Field expects int");
                        DROP();
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                    *(int *)ptr = value.asInt();
                    break;
                case FieldType::UINT:
                    if (!value.isUInt())
                    {
                        runtimeError("Field expects uint");
                        DROP();
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                    *(uint32 *)ptr = value.asUInt();
                    break;
                case FieldType::FLOAT:
                {
                    if (!value.isFloat())
                    {
                        runtimeError("Field expects float");
                        DROP();
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                    *(float *)ptr = value.asFloat();
                    break;
                }
                case FieldType::DOUBLE:
                    if (!value.isDouble())
                    {
                        runtimeError("Field expects double");
                        DROP();
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                    *(double *)ptr = value.asDouble();
                    break;

                case FieldType::BOOL:
                    if (!value.isBool())
                    {
                        runtimeError("Field expects bool");
                        DROP();
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                    *(bool *)ptr = value.asBool();
                    break;

                case FieldType::POINTER:
                    if (!value.isPointer())
                    {
                        runtimeError("Field expects pointer");
                        DROP();
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                    *(void **)ptr = value.asPointer();
                    break;

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
                    break;
                }
                }

                DROP(); // Remove object
                break;
            }

            runtimeError("Cannot 'set' property on this type");
            printf("[Object: '");
            printValue(object);
            printf("' Property : '");
            printValue(nameValue);

            printf("']\n");

            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};

            break;
        }
        case OP_INVOKE:
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
                break;
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
                    break;
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
                        break;
                    }
                    else
                    {
                        Value result = arr->values.back();
                        arr->values.pop();
                        ARGS_CLEANUP();
                        PUSH(result);
                    }
                    break;
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
                        break;
                    }
                    else
                    {
                        Value result = arr->values.back();
                        ARGS_CLEANUP();
                        PUSH(result);
                    }
                    break;
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
                    break;
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
                    break;
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
                        break;
                    }

                    bool exists = map->table.exist(key.asString());
                    ARGS_CLEANUP();
                    PUSH(makeBool(exists));
                    break;
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
                        break;
                    }

                    //  HashMap não tem remove, mas podes setar para nil
                    map->table.set(key.asString(), makeNil());
                    ARGS_CLEANUP();
                    PUSH(makeNil());
                    break;
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
                    break;
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
                    break;
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
                    break;
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
                    break;
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

                    break;
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

                break;
            }

            runtimeError("Type does not support method calls");

            printf(": ");
            printValue(receiver);
            printf("\n");

            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }

        case OP_SUPER_INVOKE:
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
            break;
        }

        case OP_GOSUB:
        {
            int16 off = (int16)READ_SHORT(); // lê u16 mas cast para signed
            if (fiber->gosubTop >= GOSUB_MAX)
                runtimeError("gosub stack overflow");
            fiber->gosubStack[fiber->gosubTop++] = ip; // retorno
            ip += off;                                 // forward/back
            break;
        }

        case OP_RETURN_SUB:
        {
            if (fiber->gosubTop > 0)
            {
                ip = fiber->gosubStack[--fiber->gosubTop];
                break;
            }
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        case OP_DEFINE_ARRAY:
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
            break;
        }
        case OP_DEFINE_MAP:
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
                    break;
                }

                inst->table.set(key.asString(), value);
            }

            PUSH(map);
            break;
        }
        case OP_SET_INDEX:
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
                    break;
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
                break;
            }

            // === MAP  ===
            if (container.isMap())
            {
                if (!index.isString())
                {
                    runtimeError("Map key must be string");
                    PUSH(value);
                    break;
                }

                MapInstance *map = container.asMap();
                map->table.set(index.asString(), value);

                PUSH(value); // Assignment returns value
                break;
            }

            // STRING é imutável!
            if (container.isString())
            {
                runtimeError("Strings are immutable");
                PUSH(value);
                break;
            }

            runtimeError("Cannot index assign this type");
            PUSH(value);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            break;
        }
        case OP_GET_INDEX:
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
                    break;
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
                break;
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
                break;
            }

            // === MAP   ===
            if (container.isMap())
            {
                if (!index.isString())
                {
                    runtimeError("Map key must be string");
                    PUSH(makeNil());
                    break;
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
                break;
            }

            runtimeError("Cannot index this type");
            PUSH(makeNil());
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        case OP_ITER_NEXT:
        {

            Value iter = POP();
            Value seq = POP();

            if (!seq.isArray())
            {
                runtimeError(" Iterator next Type is not iterable");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            ArrayInstance *array = seq.as.array;
            int index = iter.isNil() ? 0 : iter.as.integer + 1;

            if (index < (int)array->values.size())
            {
                PUSH(makeInt(index));
                PUSH(makeBool(true));
            }
            else
            {
                PUSH(makeNil());
                PUSH(makeBool(false));
            }

            //  printStack();
            break;
        }

        case OP_ITER_VALUE:
        {

            Value iter = POP();
            Value seq = POP();

            if (!seq.isArray())
            {
                runtimeError("Iterator Type is not iterable");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            ArrayInstance *array = seq.as.array;
            int index = iter.as.integer;

            if (index < 0 || index >= (int)array->values.size())
            {
                runtimeError("Iterator out of bounds");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            PUSH(array->values[index]);

            break;
        }

            // 1. OP_COPY2: Duplica os 2 topos
        case OP_COPY2:
        {

            Value b = NPEEK(0);
            Value a = NPEEK(1);
            PUSH(a);
            PUSH(b);

            break;
        }

        // 2. OP_SWAP: Troca os 2 topos
        case OP_SWAP:
        {
            Value a = POP();
            Value b = POP();
            PUSH(a);
            PUSH(b);
            break;
        }

        case OP_DISCARD:
        {
            uint8_t count = READ_BYTE();
            fiber->stackTop -= count;
            break;
        }

        case OP_TRY:
        {
            uint16_t catchAddr = READ_SHORT();
            uint16_t finallyAddr = READ_SHORT();

            if (fiber->tryDepth >= TRY_MAX)
            {
                runtimeError("Try-catch nesting too deep");
                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            TryHandler &handler = fiber->tryHandlers[fiber->tryDepth];
            handler.catchIP = catchAddr == 0xFFFF ? nullptr : func->chunk->code + catchAddr;
            handler.finallyIP = finallyAddr == 0xFFFF ? nullptr : func->chunk->code + finallyAddr;
            handler.stackRestore = fiber->stackTop;
            handler.inFinally = false;
            handler.pendingError = makeNil();
            handler.hasPendingError = false;
            handler.catchConsumed = false;

            fiber->tryDepth++;
            break;
        }

        case OP_POP_TRY:
        {
            if (fiber->tryDepth > 0)
            {
                fiber->tryDepth--;
            }
            break;
        }

        case OP_ENTER_CATCH:
        {
            if (fiber->tryDepth > 0)
            {
                fiber->tryHandlers[fiber->tryDepth - 1].hasPendingError = false;
            }
            break;
        }

        case OP_ENTER_FINALLY:
        {
            if (fiber->tryDepth > 0)
            {
                fiber->tryHandlers[fiber->tryDepth - 1].inFinally = true;
            }
            break;
        }

        case OP_THROW:
        {
            Value error = POP();
            bool handlerFound = false;

            while (fiber->tryDepth > 0)
            {
                TryHandler &handler = fiber->tryHandlers[fiber->tryDepth - 1];

                if (handler.inFinally)
                {
                    handler.pendingError = error;
                    handler.hasPendingError = true;
                    fiber->tryDepth--;
                    continue;
                }

                fiber->stackTop = handler.stackRestore;

                // Tem catch?
                if (handler.catchIP != nullptr && !handler.catchConsumed)
                {
                    handler.catchConsumed = true;

                    PUSH(error);
                    ip = handler.catchIP;
                    handlerFound = true;

                    break;
                }

                // Só finally?
                else if (handler.finallyIP != nullptr)
                {
                    handler.pendingError = error;
                    handler.hasPendingError = true;
                    handler.inFinally = true;
                    ip = handler.finallyIP;
                    handlerFound = true;
                    break;
                }

                fiber->tryDepth--;
            }

            if (!handlerFound)
            {
                char buffer[256];
                valueToBuffer(error, buffer, sizeof(buffer));
                runtimeError("Uncaught exception: %s", buffer);

                return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
            }

            break;
        }

        case OP_EXIT_FINALLY:
        {
            if (fiber->tryDepth > 0)
            {
                TryHandler &handler = fiber->tryHandlers[fiber->tryDepth - 1];
                handler.inFinally = false;

                if (handler.hasPendingReturn)
                {
                    Value returnValue = handler.pendingReturn;
                    handler.hasPendingReturn = false;
                    fiber->tryDepth--;

                    // Procura próximo finally
                    bool hasAnotherFinally = false;
                    for (int depth = fiber->tryDepth - 1; depth >= 0; depth--)
                    {
                        TryHandler &next = fiber->tryHandlers[depth];
                        if (next.finallyIP != nullptr && !next.inFinally)
                        {
                            next.pendingReturn = returnValue;
                            next.hasPendingReturn = true;
                            next.inFinally = true;
                            fiber->tryDepth = depth + 1;
                            ip = next.finallyIP;
                            hasAnotherFinally = true;
                            break;
                        }
                    }

                    if (!hasAnotherFinally)
                    {
                        // Executa return de verdade
                        fiber->frameCount--;

                        if (fiber->frameCount == 0)
                        {
                            fiber->stackTop = fiber->stack;
                            *fiber->stackTop++ = returnValue;
                            fiber->state = FiberState::DEAD;

                            if (fiber == &currentProcess->fibers[0])
                            {
                                for (int i = 0; i < currentProcess->nextFiberIndex; i++)
                                {
                                    currentProcess->fibers[i].state = FiberState::DEAD;
                                }
                                currentProcess->state = FiberState::DEAD;
                            }

                            STORE_FRAME();
                            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                        }

                        CallFrame *finished = &fiber->frames[fiber->frameCount];
                        fiber->stackTop = finished->slots;
                        *fiber->stackTop++ = returnValue;

                        LOAD_FRAME();
                    }

                    break;
                }

                if (handler.hasPendingError)
                {
                    Value error = handler.pendingError;
                    handler.hasPendingError = false;
                    fiber->tryDepth--;

                    // Re-throw: procura próximo handler
                    bool handlerFound = false;

                    for (int depth = fiber->tryDepth - 1; depth >= 0; depth--)
                    {
                        TryHandler &nextHandler = fiber->tryHandlers[depth];

                        if (nextHandler.inFinally)
                        {
                            nextHandler.pendingError = error;
                            nextHandler.hasPendingError = true;
                            continue;
                        }

                        fiber->stackTop = nextHandler.stackRestore;

                        if (nextHandler.catchIP != nullptr && !nextHandler.catchConsumed)
                        {
                            nextHandler.catchConsumed = true;
                            PUSH(error);
                            ip = nextHandler.catchIP;
                            handlerFound = true;
                            fiber->tryDepth = depth + 1;
                            break;
                        }
                        else if (nextHandler.finallyIP != nullptr)
                        {
                            nextHandler.pendingError = error;
                            nextHandler.hasPendingError = true;
                            nextHandler.inFinally = true;
                            ip = nextHandler.finallyIP;
                            handlerFound = true;
                            fiber->tryDepth = depth + 1;
                            break;
                        }
                    }

                    if (!handlerFound)
                    {
                        char buffer[256];
                        valueToBuffer(error, buffer, sizeof(buffer));
                        runtimeError("Uncaught exception: %s", buffer);
                        return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
                    }
                }
                else
                {
                    // Sem erro nem return pendente, só remove handler
                    fiber->tryDepth--;
                }
            }
            break;
        }

        default:
        {
            Debug::dumpFunction(func);
            runtimeError("Unknown opcode %d", instruction);
            return {FiberResult::FIBER_DONE, instructionsRun, 0, 0};
        }
        }
    }

    // Cleanup macros

#undef READ_BYTE
#undef READ_SHORT
}

#endif // !USE_COMPUTED_GOTO