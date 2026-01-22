
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
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
     
}
int main( int argc, char **argv)
{

    Interpreter vm;
  

    VMHooks hooks;
    hooks.onStart = onStart;
    hooks.onUpdate = onUpdate;
    hooks.onDestroy = onDestroy;
    hooks.onRender = onRender;

    vm.registerAll();
    SDLBindings::registerAll(vm);

    vm.setHooks(hooks);
    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scrips";
    ctx.searchPaths[2] = ".";
    ctx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    if (argc > 1)
    {
        std::ifstream file(argv[1]);
        std::string code((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
        if (!vm.run(code.c_str(), false))
        {
            printf("Error running code.\n");
            return 1;
        }
        return 0;
    }


    std::ifstream file("main_sdl.bu");
    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

                     
                     if (!vm.run(code.c_str(), false))
                     {
                         printf("Error running code.\n");
                         return 1;
                        }
                        
      

 

    return 0;
}
