
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include "bidings.hpp"
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




int main()
{

    Interpreter vm;
   vm.registerAll();
   

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
            .addFunction("DrawStats", RaylibBindings::native_DrawStats, 2)
            .addFunction("DrawPixel", RaylibBindings::native_DrawPixel, 3)
            .addFunction("DrawText", RaylibBindings::native_DrawText, 5)
            .addFunction("DrawLine", RaylibBindings::native_DrawLine, 5)
            .addFunction("DrawCircle", RaylibBindings::native_DrawCircle, 4)
            .addFunction("DrawRectangle", RaylibBindings::native_DrawRectangle, 5)
            .addFunction("DrawRectangleRec", RaylibBindings::native_DrawRectangleRec, 2)
            .addFunction("DrawTexture", RaylibBindings::native_DrawTexture, 4)
            .addFunction("DrawTextureRotate", RaylibBindings::native_DrawTextureRotateScale, 6)
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
    //vm.registerAll();
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
