
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
        snprintf(buffer, 256, "%.2f", v.as.real);
        out += buffer;
        break;
    case ValueType::DOUBLE:
        snprintf(buffer, 256, "%.2f", v.as.number);
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
        return vm->makeNil();
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

    return vm->makeString(result.c_str());
}

Value native_write(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("write expects string as first argument");
        return vm->makeNil();
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

    printf("%s", result.c_str());
    return vm->makeNil();
}

 


Value native_gc(Interpreter *vm, int argCount, Value *args)
{
    vm->runGC();
    return vm->makeNil();
}

 

Value native_rand(Interpreter *vm, int argCount, Value *args)
{

    if (argCount == 0)
    {
        return vm->makeDouble(RandomGenerator::instance().randFloat());
    }
    else if (argCount == 1)
    {
        double value = args[0].asDouble();
        return vm->makeDouble(RandomGenerator::instance().randFloat(0, value));
    }
    else
    {
        double min = args[0].asDouble();
        double max = args[1].asDouble();
        return vm->makeDouble(RandomGenerator::instance().randFloat(min, max));
    }
    return vm->makeNil();
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

int main()
{

    Interpreter vm;

    vm.addModule("math")
        .addDouble("PI", 3.14159265358979)
        .addDouble("E", 2.71828182845905)
        .addFloat("SQRT2", 1.41421356f)
        .addInt("MAX_INT", 2147483647)
        .addFunction("rand", native_rand, -1);

    vm.registerNative("_gc", native_gc, 0);

    vm.registerNative("write", native_write, -1);
    vm.registerNative("format", native_format, -1);
   

    bool useModule = true;

    if (useModule)
    {
        vm.addModule("raylib")
            .addFunction("InitWindow", RaylibBindings::native_InitWindow, 3)
            .addFunction("CloseWindow", RaylibBindings::native_CloseWindow, 0)
            .addFunction("WindowShouldClose", RaylibBindings::native_WindowShouldClose, 0)
            .addFunction("SetTargetFPS", RaylibBindings::native_SetTargetFPS, 1)
            .addFunction("GetFPS", RaylibBindings::native_GetFPS, 0)
            .addFunction("GetFrameTime", RaylibBindings::native_GetFrameTime, 0)
            .addFunction("BeginDrawing", RaylibBindings::native_BeginDrawing, 0)
            .addFunction("EndDrawing", RaylibBindings::native_EndDrawing, 0)
            .addFunction("ClearBackground", RaylibBindings::native_ClearBackground, 1)
            .addFunction("DrawFPS", RaylibBindings::native_DrawFps, 2)
            .addFunction("DrawPixel", RaylibBindings::native_DrawPixel, 3)
            .addFunction("DrawText", RaylibBindings::native_DrawText, 5)
            .addFunction("DrawLine", RaylibBindings::native_DrawLine, 4)
            .addFunction("DrawCircle", RaylibBindings::native_DrawCircle, 4)
            .addFunction("DrawRectangle", RaylibBindings::native_DrawRectangle, 5)
            .addFunction("DrawRectangleRec", RaylibBindings::native_DrawRectangleRec, 2)
            .addFunction("DrawTexture", RaylibBindings::native_DrawTexture, 4)
            .addFunction("LoadTexture", RaylibBindings::native_LoadTexture, 1)
            .addFunction("UnloadTexture", RaylibBindings::native_UnloadTexture, 1)
            .addFunction("GetMousePosition", RaylibBindings::native_GetMousePosition, 0)
            .addFunction("IsMouseButtonDown", RaylibBindings::native_IsMouseButtonDown, 1)
            .addFunction("IsMouseButtonPressed", RaylibBindings::native_IsMouseButtonPressed, 1)
            .addFunction("IsMouseButtonReleased", RaylibBindings::native_IsMouseButtonReleased, 1)
            .addFunction("GetMouseX", RaylibBindings::native_GetMouseX, 0)
            .addFunction("GetMouseY", RaylibBindings::native_GetMouseY, 0);
    }
    else
    {

        vm.registerNative("InitWindow", RaylibBindings::native_InitWindow, 3);
        vm.registerNative("CloseWindow", RaylibBindings::native_CloseWindow, 0);
        vm.registerNative("WindowShouldClose", RaylibBindings::native_WindowShouldClose, 0);
        vm.registerNative("SetTargetFPS", RaylibBindings::native_SetTargetFPS, 1);
        vm.registerNative("GetFPS", RaylibBindings::native_GetFPS, 0);
        vm.registerNative("GetFrameTime", RaylibBindings::native_GetFrameTime, 0);
        vm.registerNative("BeginDrawing", RaylibBindings::native_BeginDrawing, 0);
        vm.registerNative("EndDrawing", RaylibBindings::native_EndDrawing, 0);
        vm.registerNative("ClearBackground", RaylibBindings::native_ClearBackground, 1);
        vm.registerNative("DrawFPS", RaylibBindings::native_DrawFps, 2);
        vm.registerNative("DrawPixel", RaylibBindings::native_DrawPixel, 3);
        vm.registerNative("DrawText", RaylibBindings::native_DrawText, 5);
        vm.registerNative("DrawLine", RaylibBindings::native_DrawLine, 4);
        vm.registerNative("DrawCircle", RaylibBindings::native_DrawCircle, 4);
        vm.registerNative("DrawRectangle", RaylibBindings::native_DrawRectangle, 5);
        vm.registerNative("DrawRectangleRec", RaylibBindings::native_DrawRectangleRec, 2);
        vm.registerNative("DrawTexture", RaylibBindings::native_DrawTexture, 4);
        vm.registerNative("LoadTexture", RaylibBindings::native_LoadTexture, 1);
        vm.registerNative("UnloadTexture", RaylibBindings::native_UnloadTexture, 1);
        vm.registerNative("GetMousePosition", RaylibBindings::native_GetMousePosition, 0);
        vm.registerNative("IsMouseButtonDown", RaylibBindings::native_IsMouseButtonDown, 1);
        vm.registerNative("IsMouseButtonPressed", RaylibBindings::native_IsMouseButtonPressed, 1);
        vm.registerNative("IsMouseButtonReleased", RaylibBindings::native_IsMouseButtonReleased, 1);
        vm.registerNative("GetMouseX", RaylibBindings::native_GetMouseX, 0);
        vm.registerNative("GetMouseY", RaylibBindings::native_GetMouseY, 0);
    }

    // printf("Modules registered: %zu\n", vm.modules.size());
    // printf("Module 'raylib' functions: %zu\n", vm.modules[0]->functions.size());

    // printf("Global natives registered: %zu\n", vm.natives.size());

    RaylibBindings::registerColor(vm);
    RaylibBindings::registerRectangle(vm);
    RaylibBindings::registerVector2(vm);
    // RaylibBindings::registerVector3(vm);
    // RaylibBindings::registerCamera2D(vm);

    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scrips";
    ctx.searchPaths[2] = ".";
    ctx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    std::ifstream file("game.bu");
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());


                     
                     if (!vm.run(code.c_str(), false))
                     {
                         std::cerr << "Error running code.\n";
                         return 1;
                        }
                        
                        
         //               vm.dumpToFile("dump.txt");

    // for (int i = 0; i < 5; i++)
    // {
    //     // printf("\n=== FRAME %d ===\n", i);
    //     vm.update(0.016f);

    //     // Pausa para ver output
    //     std::this_thread::sleep_for(std::chrono::milliseconds(16));
    // }

    return 0;
}
