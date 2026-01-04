
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include "random.hpp"
#include "interpreter.hpp"
#include "platform.hpp"

static void valueToString(const Value &v, std::string &out)
{
    char buffer[256];

    switch (v.type)
    {
    case ValueType::NIL:
        out += "nil";
        break;
    case ValueType::BOOL:
        out += v.as.boolean ? "true" : "false";
        break;
    case ValueType::BYTE:
        snprintf(buffer, 256, "%u", v.as.byte);
        out += buffer;
        break;
    case ValueType::INT:
        snprintf(buffer, 256, "%d", v.as.integer);
        out += buffer;
        break;
    case ValueType::UINT:
        snprintf(buffer, 256, "%u", v.as.unsignedInteger);
        out += buffer;
        break;
    case ValueType::FLOAT:
        snprintf(buffer, 256, "%.4f", v.as.real);
        out += buffer;
        break;
    case ValueType::DOUBLE:
        snprintf(buffer, 256, "%.4f", v.as.number);
        out += buffer;
        break;
    case ValueType::STRING:
        out += v.as.string->chars();
        break;
    case ValueType::ARRAY:
        out += "[array]";
        break;
    case ValueType::MAP:
        out += "{map}";
        break;
    default:
        out += "<object>";
    }
}

Value native_format(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("format expects string as first argument");
        return Value::makeNil();
    }

    const char *fmt = args[0].as.string->chars();
    std::string result;
    int argIndex = 1;

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] == '{' && fmt[i + 1] == '}')
        {
            if (argIndex < argCount)
            {
                valueToString(args[argIndex++], result);
            }
            i++;
        }
        else
        {
            result += fmt[i];
        }
    }

    return Value::makeString(result.c_str());
}

Value native_write(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("write expects string as first argument");
        return Value::makeNil();
    }

    const char *fmt = args[0].as.string->chars();
    std::string result;
    int argIndex = 1;

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] == '{' && fmt[i + 1] == '}')
        {
            if (argIndex < argCount)
            {
                valueToString(args[argIndex++], result);
            }
            i++;
        }
        else
        {
            result += fmt[i];
        }
    }

    OsPrintf("%s", result.c_str());
    return Value::makeNil();
}

Value native_sqrt(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1)
    {
        vm->runtimeError("sqrt expects 1 argument");
        return Value::makeNil();
    }

    double value;
    if (args[0].isInt())
        value = (double)args[0].asInt();
    else if (args[0].isDouble())
        value = args[0].asDouble();
    else
    {
        vm->runtimeError("sqrt expects a number");
        return Value::makeNil();
    }

    if (value < 0)
    {
        vm->runtimeError("sqrt of negative number");
        return Value::makeNil();
    }

    return Value::makeDouble(std::sqrt(value));
}

Value native_sin(Interpreter *vm, int argCount, Value *args)
{
    double x = args[0].isInt() ? (double)args[0].asInt() : args[0].asDouble();
    return Value::makeDouble(std::sin(x));
}

Value native_cos(Interpreter *vm, int argCount, Value *args)
{
    double x = args[0].isInt() ? (double)args[0].asInt() : args[0].asDouble();
    return Value::makeDouble(std::cos(x));
}

Value native_abs(Interpreter *vm, int argCount, Value *args)
{
    if (args[0].isInt())
        return Value::makeInt(std::abs(args[0].asInt()));
    else
        return Value::makeDouble(std::fabs(args[0].asDouble()));
}

Value native_clock(Interpreter *vm, int argCount, Value *args)
{
    return Value::makeDouble(static_cast<double>(clock()) / CLOCKS_PER_SEC);
}

 

Value native_rand(Interpreter *vm, int argCount, Value *args)
{

    if (argCount == 0)
    {
        return Value::makeDouble(RandomGenerator::instance().randFloat());
    }
    else if (argCount == 1)
    {
        double value = args[0].asDouble();
        return Value::makeDouble(RandomGenerator::instance().randFloat(0, value));
    }
    else
    {
        double min = args[0].asDouble();
        double max = args[1].asDouble();
        return Value::makeDouble(RandomGenerator::instance().randFloat(min, max));
    }
    return Value::makeNil();
}

struct FileLoaderContext
{
    const char *searchPaths[8];
    int pathCount;
    char fullPath[512];
    char buffer[1024 * 1024];
};

const char *multiPathFileLoader(const char *filename, size_t *outSize, void *userdata)
{
    FileLoaderContext *ctx = (FileLoaderContext *)userdata;

    for (int i = 0; i < ctx->pathCount; i++)
    {
        snprintf(ctx->fullPath, sizeof(ctx->fullPath),
                 "%s/%s", ctx->searchPaths[i], filename);

        FILE *f = fopen(ctx->fullPath, "rb");
        if (!f)
        {
            continue; // Tenta próximo path
        }

        // Obtém size
        fseek(f, 0, SEEK_END);
        long size = ftell(f);

        if (size <= 0)
        {
            fprintf(stderr, "Empty or error reading: %s (size=%ld)\n",
                    ctx->fullPath, size);
            fclose(f);
            continue; // Tenta próximo path
        }

        if (size >= sizeof(ctx->buffer))
        {
            fprintf(stderr, "File too large: %s (%ld bytes, max %zu)\n",
                    ctx->fullPath, size, sizeof(ctx->buffer));
            fclose(f);
            *outSize = 0;
            return nullptr;
        }

        fseek(f, 0, SEEK_SET);

        size_t bytesRead = fread(ctx->buffer, 1, size, f);
        fclose(f);

        if (bytesRead != (size_t)size)
        {
            fprintf(stderr, "Read error: %s (expected %ld, got %zu)\n",
                    ctx->fullPath, size, bytesRead);
            continue;
        }

        ctx->buffer[bytesRead] = '\0';

        printf("✓ Loaded: %s (%zu bytes)\n", ctx->fullPath, bytesRead);

        *outSize = bytesRead;
        return ctx->buffer;
    }

    fprintf(stderr, "✗ File not found in any path: %s\n", filename);
    fprintf(stderr, "  Searched paths:\n");
    for (int i = 0; i < ctx->pathCount; i++)
    {
        fprintf(stderr, "    - %s/%s\n", ctx->searchPaths[i], filename);
    }

    *outSize = 0;
    return nullptr;
}

// ============================================
// TIMER FAKE (estrutura simples)
// ============================================

struct Timer
{
    double startTime;
    double duration;
    bool active;
};

// Storage fake
static Vector<Timer> timers;
static double fakeTime = 0.0; // Tempo simulado

// ============================================
// NATIVES FAKE
// ============================================

// timer.create(duration) -> id
Value native_timer_create(Interpreter *vm, int argCount, Value *args)
{
    double duration = args[0].asDouble();

    Timer t;
    t.startTime = fakeTime;
    t.duration = duration;
    t.active = true;

    int id = timers.size();
    timers.push(t);

    printf("[TIMER] Created timer %d with duration %.2f\n", id, duration);

    return Value::makeInt(id);
}

// timer.elapsed(id) -> bool
Value native_timer_elapsed(Interpreter *vm, int argCount, Value *args)
{
    int id = args[0].asInt();

    if (id < 0 || id >= timers.size())
    {
        printf("[TIMER] Invalid timer ID: %d\n", id);
        return Value::makeBool(false);
    }

    Timer &t = timers[id];
    if (!t.active)
    {
        return Value::makeBool(false);
    }

    double elapsed = fakeTime - t.startTime;
    bool done = elapsed >= t.duration;

    printf("[TIMER] Timer %d: elapsed=%.2f, duration=%.2f, done=%d\n",
           id, elapsed, t.duration, done);

    return Value::makeBool(done);
}

// timer.reset(id)
Value native_timer_reset(Interpreter *vm, int argCount, Value *args)
{
    int id = args[0].asInt();

    if (id >= 0 && id < timers.size())
    {
        timers[id].startTime = fakeTime;
        printf("[TIMER] Reset timer %d\n", id);
    }

    return Value::makeNil();
}

// timer.destroy(id)
Value native_timer_destroy(Interpreter *vm, int argCount, Value *args)
{
    int id = args[0].asInt();

    if (id >= 0 && id < timers.size())
    {
        timers[id].active = false;
        printf("[TIMER] Destroyed timer %d\n", id);
    }

    return Value::makeNil();
}

// ============================================
// HELPER - Avançar tempo (para testes)
// ============================================

void advanceTime(double seconds)
{
    fakeTime += seconds;
    printf("[TIME] Advanced to %.2f seconds\n", fakeTime);
}

 

void registerTimerModule(Interpreter *vm)
{
    // uint16 timerId = vm->defineModule("timer");
    // ModuleDef *timer = vm->getModule(timerId);

    // // Adiciona funções
    // uint16 createId = timer->addFunction("create", native_timer_create, 1);
    // uint16 elapsedId = timer->addFunction("elapsed", native_timer_elapsed, 1);
    // uint16 resetId = timer->addFunction("reset", native_timer_reset, 1);
    // uint16 destroyId = timer->addFunction("destroy", native_timer_destroy, 1);

     vm->addModule("timer")
        .addFunction("create", native_timer_create, 1)
        .addFunction("elapsed", native_timer_elapsed, 1)
        .addFunction("reset", native_timer_reset, 2)
        .addFunction("destroy", native_timer_destroy, 1);

}

int main()
{

    Interpreter vm;

    vm.registerNative("write", native_write, -1);
    vm.registerNative("format", native_format, -1);
    // vm.registerNative("sqrt", native_sqrt, 1);
     vm.registerNative("clock", native_clock, 0);
    // vm.registerNative("sin", native_sin, 1);
    // vm.registerNative("cos", native_cos, 1);
    // vm.registerNative("abs", native_abs, 1);

    // vm.registerNative("rand", native_rand, -1);


    vm.addModule("math")
        .addDouble("PI", 3.14159265358979)
        .addDouble("E", 2.71828182845905)
        .addFloat("SQRT2", 1.41421356f)
        .addInt("MAX_INT", 2147483647)
        .addFunction("sin",  native_sin , 1)
        .addFunction("cos",  native_cos , 1)
        .addFunction("sqrt", native_sqrt, 1)
        .addFunction("abs",  native_abs , 1)
        .addFunction("rand", native_rand, -1);

    registerTimerModule(&vm);

    // NativeStructDef* input = vm.registerNativeStruct("Input", sizeof(InputState));
    // vm.addStructField(input, "KEY_A", offsetof(InputState, KEY_A), FieldType::INT, true);
    // vm.addStructField(input, "KEY_B", offsetof(InputState, KEY_B), FieldType::INT, true);

    // InputState inputState = { .KEY_A = 65, .KEY_B = 66};
    // vm.addGlobal("Input", Value::makeNativeStruct(input->));

    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scrips";
    ctx.searchPaths[2] = ".";
    ctx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    std::ifstream file("main.bu");
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    // vm.compile(code.c_str(), false);

    if (!vm.run(code.c_str(), false))
    {
        std::cerr << "Error running code.\n";
        return 1;
    }

    // Chama update() do BuLang
    if (vm.functionExists("update"))
    {
        vm.callFunction("update", 0);
    }

    // Chama draw() do BuLang
    if (vm.functionExists("draw"))
    {
        vm.callFunction("draw", 0);
    }

    vm.dumpToFile("main.dump");
    return 0;
}
