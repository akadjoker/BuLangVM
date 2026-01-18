#include "interpreter.hpp"


#ifdef BU_ENABLE_PATH

#include "platform.hpp"

#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

Value native_path_join(Interpreter *vm, int argCount, Value *args)
{
    std::string result;
    for (int i = 0; i < argCount; i++)
    {
        if (!args[i].isString()) continue;
        
        if (i > 0 && !result.empty() && result.back() != '/' && result.back() != '\\')
        {
#ifdef _WIN32
            result += '\\';
#else
            result += '/';
#endif
        }
        result += args[i].asStringChars();
    }
    return vm->makeString(result.c_str());
}

Value native_path_normalize(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();
    
    std::string path = args[0].asStringChars();
    std::string result;
    std::vector<std::string> parts;
    
    // Split por '/' ou '\\'
    size_t start = 0;
    for (size_t i = 0; i <= path.size(); i++)
    {
        if (i == path.size() || path[i] == '/' || path[i] == '\\')
        {
            if (i > start)
            {
                std::string part = path.substr(start, i - start);
                if (part == "..")
                {
                    if (!parts.empty()) parts.pop_back();
                }
                else if (part != ".")
                {
                    parts.push_back(part);
                }
            }
            start = i + 1;
        }
    }
    
    // Reconstrói
    for (size_t i = 0; i < parts.size(); i++)
    {
        if (i > 0) result += "/";
        result += parts[i];
    }
    
    return vm->makeString(result.c_str());
}

Value native_path_basename(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();
    
    std::string path = args[0].asStringChars();
    size_t pos = path.find_last_of("/\\");
    
    if (pos != std::string::npos)
        return vm->makeString(path.substr(pos + 1).c_str());
    
    return args[0];
}

Value native_path_dirname(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();
    
    std::string path = args[0].asStringChars();
    size_t pos = path.find_last_of("/\\");
    
    if (pos != std::string::npos)
        return vm->makeString(path.substr(0, pos).c_str());
    
    return vm->makeString(".");
}

Value native_path_exists(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);
    
    return vm->makeBool(OsFileExists(args[0].asStringChars()));
}


Value native_path_extname(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeString("");
    
    std::string path = args[0].asStringChars();
    size_t pos = path.find_last_of('.');
    size_t slash = path.find_last_of("/\\");
    
    // Tem '.' e está depois da última barra?
    if (pos != std::string::npos && 
        (slash == std::string::npos || pos > slash))
    {
        return vm->makeString(path.substr(pos).c_str());
    }
    
    return vm->makeString("");
}

Value native_path_isdir(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return vm->makeBool(attrs != INVALID_FILE_ATTRIBUTES && 
                       (attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return vm->makeBool(stat(path, &st) == 0 && S_ISDIR(st.st_mode));
#endif
}

Value native_path_isfile(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return vm->makeBool(attrs != INVALID_FILE_ATTRIBUTES && 
                       !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return vm->makeBool(stat(path, &st) == 0 && S_ISREG(st.st_mode));
#endif
}

void Interpreter::registerPath()
{
     addModule("path")
        .addFunction("join", native_path_join, -1)
        .addFunction("normalize", native_path_normalize, 1)
        .addFunction("dirname", native_path_dirname, 1)
        .addFunction("basename", native_path_basename, 1)
        .addFunction("extname", native_path_extname, 1)
        .addFunction("exists", native_path_exists, 1)
        .addFunction("isdir", native_path_isdir, 1)
        .addFunction("isfile", native_path_isfile, 1);
}

#endif