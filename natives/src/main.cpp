// Exemplo de como criar classes e structs nativos em C++
// e fazer binding com BuLang VM

#include "interpreter.hpp"
#include "value.hpp"
#include <stdio.h>
#include <math.h>
#include <cstring>
#include <chrono>

// ============================================
// 1. STRUCTS NATIVOS (POD - Plain Old Data)
// ============================================

// Vector2 - Struct simples com 2 floats
struct Vector2
{
    float x;
    float y;
};

// Construtor do Vector2
static void vector2_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
{
    (void)vm;
    Vector2 *vec = static_cast<Vector2 *>(buffer);
    vec->x = (argc > 0) ? args[0].asNumber() : 0.0f;
    vec->y = (argc > 1) ? args[1].asNumber() : 0.0f;
}

// Vector3 - Struct com 3 floats
struct Vector3
{
    float x;
    float y;
    float z;
};

static void vector3_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
{
    (void)vm;
    Vector3 *vec = static_cast<Vector3 *>(buffer);
    vec->x = (argc > 0) ? args[0].asNumber() : 0.0f;
    vec->y = (argc > 1) ? args[1].asNumber() : 0.0f;
    vec->z = (argc > 2) ? args[2].asNumber() : 0.0f;
}

// Rectangle - Struct com 4 floats
struct Rectangle
{
    float x;
    float y;
    float width;
    float height;
};

static void rectangle_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
{
    (void)vm;
    Rectangle *rect = static_cast<Rectangle *>(buffer);
    rect->x = (argc > 0) ? args[0].asNumber() : 0.0f;
    rect->y = (argc > 1) ? args[1].asNumber() : 0.0f;
    rect->width = (argc > 2) ? args[2].asNumber() : 0.0f;
    rect->height = (argc > 3) ? args[3].asNumber() : 0.0f;
}

// Color - Struct com 4 bytes (RGBA)
struct Color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

static void color_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
{
    (void)vm;
    Color *color = static_cast<Color *>(buffer);
    color->r = (argc > 0) ? static_cast<uint8_t>(args[0].asNumber()) : 0;
    color->g = (argc > 1) ? static_cast<uint8_t>(args[1].asNumber()) : 0;
    color->b = (argc > 2) ? static_cast<uint8_t>(args[2].asNumber()) : 0;
    color->a = (argc > 3) ? static_cast<uint8_t>(args[3].asNumber()) : 255;
}

// ============================================
// 2. CLASSE NATIVA COMPLETA (com ponteiro)
// ============================================

class NativeTimer
{
public:
    std::chrono::steady_clock::time_point startTime;
    double elapsed;
    bool running;
    bool loop;  // keyword as property name test

    NativeTimer() : elapsed(0), running(false), loop(false) {}

    void start()
    {
        startTime = std::chrono::steady_clock::now();
        running = true;
    }

    void stop()
    {
        if (running)
        {
            auto now = std::chrono::steady_clock::now();
            elapsed += std::chrono::duration<double>(now - startTime).count();
            running = false;
        }
    }

    void reset()
    {
        elapsed = 0;
        running = false;
    }

    double getElapsed()
    {
        double total = elapsed;
        if (running)
        {
            auto now = std::chrono::steady_clock::now();
            total += std::chrono::duration<double>(now - startTime).count();
        }
        return total;
    }
};

// Constructor - assinatura: void*(*)(Interpreter*, int, Value*)
static void *timer_constructor(Interpreter *vm, int argCount, Value *args)
{
    (void)vm;
    (void)args;
    (void)argCount;
    NativeTimer *timer = new NativeTimer();
    return timer;
}

// Destructor - assinatura: void(*)(Interpreter*, void*)
static void timer_destructor(Interpreter *vm, void *ptr)
{
    (void)vm;
    delete static_cast<NativeTimer *>(ptr);
}

// Método: start() - assinatura: int(*)(Interpreter*, void*, int, Value*)
static int timer_start(Interpreter *vm, void *ptr, int argCount, Value *args)
{
    (void)vm;
    (void)args;
    (void)argCount;
    NativeTimer *timer = static_cast<NativeTimer *>(ptr);
    timer->start();
    return 0; // 0 return values
}

// Método: stop()
static int timer_stop(Interpreter *vm, void *ptr, int argCount, Value *args)
{
    (void)vm;
    (void)args;
    (void)argCount;
    NativeTimer *timer = static_cast<NativeTimer *>(ptr);
    timer->stop();
    return 0;
}

// Método: reset()
static int timer_reset(Interpreter *vm, void *ptr, int argCount, Value *args)
{
    (void)vm;
    (void)args;
    (void)argCount;
    NativeTimer *timer = static_cast<NativeTimer *>(ptr);
    timer->reset();
    return 0;
}

// Propriedade: elapsed (read-only) - assinatura: Value(*)(Interpreter*, void*)
static Value timer_get_elapsed(Interpreter *vm, void *ptr)
{
    NativeTimer *timer = static_cast<NativeTimer *>(ptr);
    return vm->makeDouble(timer->getElapsed());
}

// Propriedade: loop (keyword test) - getter
static Value timer_get_loop(Interpreter *vm, void *ptr)
{
    NativeTimer *timer = static_cast<NativeTimer *>(ptr);
    return vm->makeBool(timer->loop);
}

// Propriedade: loop (keyword test) - setter
static void timer_set_loop(Interpreter *vm, void *ptr, Value value)
{
    (void)vm;
    NativeTimer *timer = static_cast<NativeTimer *>(ptr);
    timer->loop = value.asBool();
}

// ============================================
// 3. EXEMPLO: RETORNAR INSTÂNCIA DE CLASSE NATIVA
// ============================================

// Classe Container que contém outra classe
class Container
{
public:
    NativeTimer *timer;

    Container()
    {
        timer = new NativeTimer();
    }

    ~Container()
    {
        delete timer;
    }
};

// Constructor
static void *container_constructor(Interpreter *vm, int argCount, Value *args)
{
    (void)vm;
    (void)args;
    (void)argCount;
    Container *container = new Container();
    return container;
}

// Destructor
static void container_destructor(Interpreter *vm, void *ptr)
{
    (void)vm;
    delete static_cast<Container *>(ptr);
}

// Método que retorna uma instância de Timer (classe nativa já registrada)
static int container_getTimer(Interpreter *vm, void *ptr, int argCount, Value *args)
{
    (void)args;
    (void)argCount;
    Container *container = static_cast<Container *>(ptr);

    // Obter a NativeClassDef do Timer
    NativeClassDef *timerClass = nullptr;
    if (!vm->tryGetNativeClassDef("Timer", &timerClass))
    {
        // Error: Timer class not found
        return 0;
    }

    // Criar uma NativeClassInstance de Timer
    Value timerValue = vm->makeNativeClassInstance(false);
    NativeClassInstance *instance = timerValue.asNativeClassInstance();
    instance->klass = timerClass;
    instance->userData = (void *)container->timer;
    instance->ownsUserData = false; // Container é o dono do Timer, não esta instância

    vm->push(timerValue);
    return 1; // 1 return value
}

// ============================================
// 4. FUNÇÃO DE REGISTRO
// ============================================

void registerNativeBindings(Interpreter *vm)
{
    // ========================================
    // REGISTRAR STRUCTS NATIVOS
    // ========================================

    // Vector2
    NativeStructDef *vec2 = vm->registerNativeStruct(
        "Vector2",
        sizeof(Vector2),
        vector2_ctor,
        nullptr, // sem destrutor (POD)
        false);
    vm->addStructField(vec2, "x", offsetof(Vector2, x), FieldType::FLOAT);
    vm->addStructField(vec2, "y", offsetof(Vector2, y), FieldType::FLOAT);

    // Vector3
    NativeStructDef *vec3 = vm->registerNativeStruct(
        "Vector3",
        sizeof(Vector3),
        vector3_ctor,
        nullptr,
        false);
    vm->addStructField(vec3, "x", offsetof(Vector3, x), FieldType::FLOAT);
    vm->addStructField(vec3, "y", offsetof(Vector3, y), FieldType::FLOAT);
    vm->addStructField(vec3, "z", offsetof(Vector3, z), FieldType::FLOAT);

    // Rectangle
    NativeStructDef *rect = vm->registerNativeStruct(
        "Rectangle",
        sizeof(Rectangle),
        rectangle_ctor,
        nullptr,
        false);
    vm->addStructField(rect, "x", offsetof(Rectangle, x), FieldType::FLOAT);
    vm->addStructField(rect, "y", offsetof(Rectangle, y), FieldType::FLOAT);
    vm->addStructField(rect, "width", offsetof(Rectangle, width), FieldType::FLOAT);
    vm->addStructField(rect, "height", offsetof(Rectangle, height), FieldType::FLOAT);

    // Color
    NativeStructDef *color = vm->registerNativeStruct(
        "Color",
        sizeof(Color),
        color_ctor,
        nullptr,
        false);
    vm->addStructField(color, "r", offsetof(Color, r), FieldType::BYTE);
    vm->addStructField(color, "g", offsetof(Color, g), FieldType::BYTE);
    vm->addStructField(color, "b", offsetof(Color, b), FieldType::BYTE);
    vm->addStructField(color, "a", offsetof(Color, a), FieldType::BYTE);

    // ========================================
    // REGISTRAR CLASSE NATIVA (Timer)
    // ========================================

    NativeClassDef *timerClass = vm->registerNativeClass(
        "Timer",
        timer_constructor,
        timer_destructor,
        0, // sem argumentos
        false);

    // Adicionar métodos
    vm->addNativeMethod(timerClass, "start", timer_start);
    vm->addNativeMethod(timerClass, "stop", timer_stop);
    vm->addNativeMethod(timerClass, "reset", timer_reset);

    // Adicionar propriedade read-only
    vm->addNativeProperty(timerClass, "elapsed", timer_get_elapsed, nullptr);

    // Adicionar propriedade com nome de keyword (teste)
    vm->addNativeProperty(timerClass, "loop", timer_get_loop, timer_set_loop);

    // ========================================
    // REGISTRAR CLASSE CONTAINER (exemplo de retornar classe nativa)
    // ========================================

    NativeClassDef *containerClass = vm->registerNativeClass(
        "Container",
        container_constructor,
        container_destructor,
        0,
        false);

    // Método que retorna uma instância de Timer
    vm->addNativeMethod(containerClass, "getTimer", container_getTimer);
}

// ============================================
// 5. MAIN
// ============================================

#include <string>
#include <fstream>


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
        snprintf(ctx->fullPath, sizeof(ctx->fullPath), "%s/%s", ctx->searchPaths[i], filename);

        FILE *f = fopen(ctx->fullPath, "rb");
        if (!f)
            continue;

        fseek(f, 0, SEEK_END);
        long size = ftell(f);

        if (size <= 0)
        {
            fclose(f);
            continue;
        }

        if (size >= (long)sizeof(ctx->buffer))
        {
            fprintf(stderr, "File too large: %s (%ld bytes)\n", ctx->fullPath, size);
            fclose(f);
            *outSize = 0;
            return nullptr;
        }

        fseek(f, 0, SEEK_SET);
        size_t bytesRead = fread(ctx->buffer, 1, size, f);
        fclose(f);

        if (bytesRead != (size_t)size)
            continue;

        ctx->buffer[bytesRead] = '\0';
        *outSize = bytesRead;
        return ctx->buffer;
    }

    *outSize = 0;
    return nullptr;
}


static bool readFile(const char *path, std::string &out)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        return false;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    out.resize(size);
    file.read(&out[0], size);
    return true;
}

int main(int argc, char **argv)
{
    const char *path = (argc >= 2) ? argv[1] : "../scripts/main.bu";
    std::string source;


    if (!readFile(path, source))
        return 1;

    Interpreter vm;
    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "../scripts";
    ctx.searchPaths[2] = ".";
    ctx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    vm.registerAll();


    registerNativeBindings(&vm);

    vm.run(source.c_str(), false);

    // ============================================
    // PROCESS/FIBER UPDATE LOOP
    // ============================================
    // Se há processos ativos, roda o scheduler
    int maxFrames = 50000; // Safety limit
    int frame = 0;

    while (vm.getTotalAliveProcesses() > 0 && frame < maxFrames) 
    {
        vm.update(0.016f); // ~60 FPS
        frame++;
    }

    if (frame >= maxFrames) 
    {
        printf("⚠️  Warning: Test hit frame limit (%d frames)\n", maxFrames);
        return 1;
    }

    if (frame > 0)
    {
        printf("✓ Processes completed in %d frames\n", frame);
    }

    return 0;
}
