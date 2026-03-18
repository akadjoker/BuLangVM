// ============================================
// BuLang - Script Runner
// Usage: bulang <script.bu> [options]
//        bulang --version
//        bulang --help
// ============================================

#include "interpreter.hpp"
#include "version.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// ── File loader ─────────────────────────────────────────────

struct LoaderContext
{
    std::vector<std::string> searchPaths;
    std::string buffer;
};

static std::string readFile(const char *path)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs)
        return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

static const char *fileLoader(const char *filename, size_t *outSize, void *userdata)
{
    if (!filename || !outSize || !userdata)
        return nullptr;

    auto *ctx = static_cast<LoaderContext *>(userdata);
    *outSize = 0;

    // Strip leading slashes
    const char *rel = filename;
    while (*rel == '/' || *rel == '\\')
        ++rel;

    auto tryLoad = [&](const std::string &path) -> bool
    {
        std::string data = readFile(path.c_str());
        if (data.empty())
            return false;
        ctx->buffer = std::move(data);
        *outSize = ctx->buffer.size();
        return true;
    };

    // Try search paths
    for (const auto &dir : ctx->searchPaths)
    {
        if (tryLoad(dir + "/" + rel))
            return ctx->buffer.c_str();
    }

    // Fallback
    if (tryLoad(filename))
        return ctx->buffer.c_str();
    if (rel != filename && tryLoad(rel))
        return ctx->buffer.c_str();

    return nullptr;
}

// ── Helpers ─────────────────────────────────────────────────

static std::string dirname(const std::string &path)
{
    auto pos = path.find_last_of("/\\");
    if (pos == std::string::npos)
        return ".";
    return path.substr(0, pos);
}

static void printVersion()
{
    std::printf("BuLang %s\n", BUGL_VERSION_STRING);
}

static void printHelp(const char *prog)
{
    std::printf("BuLang %s - A lightweight scripting language\n\n", BUGL_VERSION_STRING);
    std::printf("Usage:\n");
    std::printf("  %s <script.bu> [options]    Run a script\n", prog);
    std::printf("  %s -e \"code\"                Execute code string\n", prog);
    std::printf("  %s --version                Show version\n", prog);
    std::printf("  %s --help                   Show this help\n\n", prog);
    std::printf("Options:\n");
    std::printf("  --dump       Dump bytecode after compilation\n");
    std::printf("  -I <path>    Add module search path\n");
    std::printf("\nExamples:\n");
    std::printf("  %s main.bu\n", prog);
    std::printf("  %s -e \"print(1 + 2);\"\n", prog);
    std::printf("  %s script.bu --dump\n", prog);
}

// ── Main ────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    const char *scriptFile = nullptr;
    const char *evalCode = nullptr;
    bool dump = false;
    std::vector<std::string> includePaths;

    // Parse arguments
    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "--version") == 0 || std::strcmp(argv[i], "-v") == 0)
        {
            printVersion();
            return 0;
        }
        else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0)
        {
            printHelp(argv[0]);
            return 0;
        }
        else if (std::strcmp(argv[i], "--dump") == 0)
        {
            dump = true;
        }
        else if (std::strcmp(argv[i], "-e") == 0)
        {
            if (i + 1 < argc)
            {
                evalCode = argv[++i];
            }
            else
            {
                std::fprintf(stderr, "Error: -e requires code argument\n");
                return 1;
            }
        }
        else if (std::strcmp(argv[i], "-I") == 0)
        {
            if (i + 1 < argc)
            {
                includePaths.push_back(argv[++i]);
            }
            else
            {
                std::fprintf(stderr, "Error: -I requires path argument\n");
                return 1;
            }
        }
        else if (argv[i][0] == '-')
        {
            std::fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
        else if (!scriptFile)
        {
            scriptFile = argv[i];
        }
    }

    // Need either script file or eval code
    if (!scriptFile && !evalCode)
    {
        printHelp(argv[0]);
        return 1;
    }

    // Get source code
    std::string source;
    if (evalCode)
    {
        source = evalCode;
    }
    else
    {
        source = readFile(scriptFile);
        if (source.empty())
        {
            std::fprintf(stderr, "Error: cannot open '%s'\n", scriptFile);
            return 1;
        }
    }

    // Set up VM
    Interpreter vm;
    vm.registerAll();

    // Configure file loader
    LoaderContext loaderCtx;
    
    // Add script directory to search paths
    if (scriptFile)
    {
        std::string scriptDir = dirname(scriptFile);
        loaderCtx.searchPaths.push_back(scriptDir);
        loaderCtx.searchPaths.push_back(scriptDir + "/..");
    }
    
    // Add user-specified paths
    for (const auto &path : includePaths)
    {
        loaderCtx.searchPaths.push_back(path);
    }
    
    // Add default paths
    loaderCtx.searchPaths.push_back("scripts");
    loaderCtx.searchPaths.push_back(".");
    
    vm.setFileLoader(fileLoader, &loaderCtx);

    // Run
    bool ok = vm.run(source.c_str(), dump);
    
    return ok ? 0 : 1;
}
