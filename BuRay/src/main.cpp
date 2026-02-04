 

#include "interpreter.hpp"
#include "value.hpp"
#include "bindings.hpp" 
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
    RaylibBindings::registerAll(vm);

 

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
