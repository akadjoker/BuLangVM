// ============================================
// BuLang Web - Main Entry Point
// Executa scripts .bu com raylib no browser
// ============================================

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "interpreter.hpp"
#include "bindings.hpp"  // RaylibBindings
#include "Outputcapture.h"

// ============================================
// Globals
// ============================================
static Interpreter* g_vm = nullptr;

// Required by platform.cpp for Emscripten builds
OutputCapture* g_currentOutput = nullptr;

// ============================================
// File Loader para includes
// ============================================
struct FileLoaderContext
{
    const char* searchPaths[8];
    int pathCount;
    char fullPath[512];
    char buffer[1024 * 1024];
};

static FileLoaderContext g_fileCtx;

const char* multiPathFileLoader(const char* filename, size_t* outSize, void* userdata)
{
    FileLoaderContext* ctx = (FileLoaderContext*)userdata;

    for (int i = 0; i < ctx->pathCount; i++)
    {
        snprintf(ctx->fullPath, sizeof(ctx->fullPath),
                 "%s/%s", ctx->searchPaths[i], filename);

        FILE* f = fopen(ctx->fullPath, "rb");
        if (!f)
        {
            continue;
        }

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
        {
            continue;
        }

        ctx->buffer[bytesRead] = '\0';
        printf("Loaded: %s (%zu bytes)\n", ctx->fullPath, bytesRead);

        *outSize = bytesRead;
        return ctx->buffer;
    }

    fprintf(stderr, "File not found: %s\n", filename);
    for (int i = 0; i < ctx->pathCount; i++)
    {
        fprintf(stderr, "  - %s/%s\n", ctx->searchPaths[i], filename);
    }

    *outSize = 0;
    return nullptr;
}

// ============================================
// Helper: Read file
// ============================================
static char* readFile(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "Could not open: %s\n", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (!buffer)
    {
        fclose(file);
        return nullptr;
    }

    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';
    fclose(file);

    return buffer;
}

// ============================================
// Main
// ============================================
int main(int argc, char* argv[])
{
    printf("=== BuLang Web ===\n");

    // Criar interpretador
    Interpreter vm;
    g_vm = &vm;

    // Registar builtins e raylib
    vm.registerAll();
    RaylibBindings::registerAll(vm);
    printf("Raylib module registered\n");

    // Configurar file loader para includes
    // No Emscripten, os assets ficam em /assets (via --preload-file)
    g_fileCtx.searchPaths[0] = "/assets";
    g_fileCtx.searchPaths[1] = "/assets/demos";
    g_fileCtx.searchPaths[2] = ".";
    g_fileCtx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &g_fileCtx);

    // Carregar e executar script principal
    const char* scriptPath = "/assets/main.bu";
    printf("Loading: %s\n", scriptPath);

    char* source = readFile(scriptPath);
    if (!source)
    {
        fprintf(stderr, "Failed to read: %s\n", scriptPath);
        return 1;
    }

    if (!vm.run(source, false))
    {
        fprintf(stderr, "Script error\n");
        free(source);
        return 1;
    }

    free(source);
    printf("Script completed\n");

    g_vm = nullptr;
    return 0;
}
