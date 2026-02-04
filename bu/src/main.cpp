// Bu - BuLang interpreter with plugin support
// Usage: bu script.bu
//        bu --plugin SDL script.bu
//        bu (runs main.bu from current directory)

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
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

void onStart(Process *proc) {}
void onUpdate(Process *proc, float dt) {}
void onDestroy(Process *proc, int exitCode) {}
void onRender(Process *proc) {}

void printUsage(const char* progName)
{
    printf("Bu - BuLang interpreter with plugin support\n");
    printf("Usage: %s [options] [script.bu]\n\n", progName);
    printf("Options:\n");
    printf("  --plugin <name>    Load a plugin before running script\n");
    printf("  --module-path <p>  Add plugin search path\n");
    printf("  --help             Show this help\n");
    printf("\nExamples:\n");
    printf("  %s main.bu                    # Run main.bu\n", progName);
    printf("  %s --plugin sdl game.bu       # Load SDL plugin, run game.bu\n", progName);
    printf("  %s                            # Run main.bu in current directory\n", progName);
    printf("\nPlugins are searched in:\n");
    printf("  1. Current directory\n");
    printf("  2. ./modules/\n");
    printf("  3. Custom paths added with --module-path\n");
}

int main(int argc, char **argv)
{
    Interpreter vm;

    VMHooks hooks;
    hooks.onStart = onStart;
    hooks.onUpdate = onUpdate;
    hooks.onDestroy = onDestroy;
    hooks.onRender = onRender;

    vm.registerAll();
    vm.setHooks(hooks);

    FileLoaderContext ctx;
    ctx.searchPaths[0] = "./bin";
    ctx.searchPaths[1] = "./scripts";
    ctx.searchPaths[2] = ".";
    ctx.pathCount = 3;
    vm.setFileLoader(multiPathFileLoader, &ctx);

    const char* scriptFile = nullptr;

    // Parse command line arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "--plugin") == 0 || strcmp(argv[i], "-p") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "Error: --plugin requires a plugin name\n");
                return 1;
            }
            const char* pluginName = argv[++i];
            if (!vm.loadPluginByName(pluginName))
            {
                fprintf(stderr, "Error loading plugin '%s': %s\n",
                        pluginName, vm.getLastPluginError());
                return 1;
            }
        }
        else if (strcmp(argv[i], "--module-path") == 0 || strcmp(argv[i], "-m") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "Error: --module-path requires a path\n");
                return 1;
            }
            vm.addPluginSearchPath(argv[++i]);
        }
        else if (argv[i][0] == '-')
        {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            printUsage(argv[0]);
            return 1;
        }
        else
        {
            scriptFile = argv[i];
        }
    }
     

    // Default to main.bu if no script specified
    if (!scriptFile)
    {
        scriptFile = "main.bu";
    }

    // Load and run script
    std::ifstream file(scriptFile);
    if (!file.is_open())
    {
        fprintf(stderr, "Error: Could not open file '%s'\n", scriptFile);
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    if (!vm.run(code.c_str(), false))
    {
        fprintf(stderr, "Error running script.\n");
        return 1;
    }

    vm.unloadAllPlugins();
    return 0;
}
