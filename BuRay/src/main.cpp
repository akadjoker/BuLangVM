
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "bidings.hpp"
#include "random.hpp"
#include "interpreter.hpp"

 

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



Value native_key(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1  )
    {
        vm->runtimeError("key expects 1 integer argument");
        return vm->makeNil();
    }

    int key = (int)args[0].asNumber();
    bool pressed = IsKeyPressed(key);
    return vm->makeBool(pressed);
}

Value native_mouseX(Interpreter *vm, int argCount, Value *args)
{
    int x = GetMouseX();
    return vm->makeInt(x);
}
Value native_mouseY(Interpreter *vm, int argCount, Value *args)
{
    int y = GetMouseY();
    return vm->makeInt(y);
}

Value native_mouse_down(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1  )
    {
        vm->runtimeError("mouse_down expects 1 integer argument");
        return vm->makeNil();
    }

    int button = (int)args[0].asNumber();
    bool down = IsMouseButtonDown(button);
    return vm->makeBool(down);
}
 

Value native_rand_range(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 2  )
    {
        vm->runtimeError("rand_range expects 2 integer arguments");
        return vm->makeNil();
    }
    int min = (int)args[0].asNumber();
    int max = (int)args[1].asNumber();
    return vm->makeInt(GetRandomValue(min, max));
}
Texture2D bunny;

void onStart(Process *proc)
{
    // printf("[start] %s id=%u\n", proc->name->chars(), proc->id);
}

void onUpdate(Process *proc, float dt)
{
    // ler posição vinda do script
   

    // printf("[update] %s pos=(%.2f, %.2f) dt=%.3f\n",
    //     proc->name->chars(), x, y, dt);

    // proc->privates[0] = vm->makeDouble(1);
    // proc->privates[1] = vm->makeDouble(1);
}

void onDestroy(Process *proc, int exitCode)
{
   // printf("[destroy] %s exit=%d\n", proc->name->chars(), proc->exitCode);
}

void onRender(Process *proc)
{
        // printf("[render] %s rendering...\n", proc->name->chars());
    float x = (float)proc->privates[0].asDouble();
    float y = (float)proc->privates[1].asDouble();

    //DrawCircle((int)x, (int)y, 20, RED);

    DrawTexture(bunny, (int)x - bunny.width / 2, (int)y - bunny.height / 2, WHITE);
}
int main()
{

    Interpreter vm;
 
   vm.registerNative("key", native_key, 1);
    vm.registerNative("mouseX", native_mouseX, 0);
    vm.registerNative("mouseY", native_mouseY, 0);
    vm.registerNative("mouse_down", native_mouse_down, 1);

    vm.registerNative("rand_range", native_rand_range, 2);

    VMHooks hooks;
    hooks.onStart = onStart;
    hooks.onUpdate = onUpdate;
    hooks.onDestroy = onDestroy;
    hooks.onRender = onRender;

    vm.setHooks(hooks);
    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scrips";
    ctx.searchPaths[2] = ".";
    ctx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    std::ifstream file("main.bu");
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());


                     
                     if (!vm.run(code.c_str(), false))
                     {
                         std::cerr << "Error running code.\n";
                         return 1;
                        }
                        
                         InitWindow(800, 600, "Game");
    SetTargetFPS(60);

    bunny = LoadTexture("assets/wabbit_alpha.png");

     if (!vm.run(code.c_str()))
    {
        std::cerr << "Error running code.\n";

        return 1;
    }

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        vm.update(dt);

        BeginDrawing();
        
        ClearBackground(BLACK);


        vm.render();

        DrawFPS(10, 10);
        DrawText(TextFormat("Processes: %d", vm.getTotalAliveProcesses()), 10, 30, 20, GREEN);

        EndDrawing();
    }

    UnloadTexture(bunny);

    CloseWindow();
 

    return 0;
}
