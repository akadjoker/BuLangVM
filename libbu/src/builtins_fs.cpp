
#include "interpreter.hpp"

#ifdef BU_ENABLE_FILE_IO

#include "platform.hpp"
#include "utils.hpp"
#include <string>
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

Value native_fs_read(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();
    
    const char *path = args[0].asStringChars();
    int size = OsFileSize(path);
    
    if (size < 0)
        return vm->makeNil();
    
    char *buffer = (char*)malloc(size + 1);
    int bytesRead = OsFileRead(path, buffer, size);
    
    if (bytesRead < 0)
    {
        free(buffer);
        return vm->makeNil();
    }
    
    buffer[bytesRead] = '\0';
    Value result = vm->makeString(buffer);
    free(buffer);
    
    return result;
}

Value native_fs_write(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString() || !args[1].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    const char *data = args[1].asStringChars();
    
    return vm->makeBool(OsFileWrite(path, data, strlen(data)) > 0);
}

Value native_fs_list(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();
    
    Value arr = vm->makeArray();
    const char *dirPath = args[0].asStringChars();
    
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    std::string searchPath = std::string(dirPath) + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (strcmp(findData.cFileName, ".") != 0 && 
                strcmp(findData.cFileName, "..") != 0)
            {
                arr.asArray()->values.push(vm->makeString(findData.cFileName));
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
#else
    DIR *dir = opendir(dirPath);
    if (dir)
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr)
        {
            if (strcmp(entry->d_name, ".") != 0 && 
                strcmp(entry->d_name, "..") != 0)
            {
                arr.asArray()->values.push(vm->makeString(entry->d_name));
            }
        }
        closedir(dir);
    }
#endif
    
    return arr;
}

Value native_fs_mkdir(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    
#ifdef _WIN32
    return vm->makeBool(CreateDirectoryA(path, NULL) != 0 || 
                       GetLastError() == ERROR_ALREADY_EXISTS);
#else
    return vm->makeBool(mkdir(path, 0755) == 0 || errno == EEXIST);
#endif
}

Value native_fs_rmdir(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    
#ifdef _WIN32
    return vm->makeBool(RemoveDirectoryA(path) != 0);
#else
    return vm->makeBool(rmdir(path) == 0);
#endif
}

Value native_fs_remove(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    
#ifdef _WIN32
    return vm->makeBool(DeleteFileA(path) != 0);
#else
    return vm->makeBool(remove(path) == 0);
#endif
}

Value native_fs_append(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString() || !args[1].isString())
        return vm->makeBool(false);
    
    const char *path = args[0].asStringChars();
    const char *data = args[1].asStringChars();
    
#ifdef _WIN32
    HANDLE hFile = CreateFileA(path, FILE_APPEND_DATA, 0, NULL, 
                               OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return vm->makeBool(false);
    
    DWORD written;
    BOOL result = WriteFile(hFile, data, strlen(data), &written, NULL);
    CloseHandle(hFile);
    return vm->makeBool(result != 0);
#else
    FILE *f = fopen(path, "a");
    if (!f) return vm->makeBool(false);
    
    size_t written = fwrite(data, 1, strlen(data), f);
    fclose(f);
    return vm->makeBool(written == strlen(data));
#endif
}

Value native_fs_stat(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();
    
    const char *path = args[0].asStringChars();
    
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &fileInfo))
        return vm->makeNil();
    
    Value map = vm->makeMap();
    MapInstance *m = map.asMap();
    
    LARGE_INTEGER size;
    size.LowPart = fileInfo.nFileSizeLow;
    size.HighPart = fileInfo.nFileSizeHigh;
    
    m->table.set(vm->makeString("size"), vm->makeInt((int)size.QuadPart));
    m->table.set(vm->makeString("isdir"), 
                 vm->makeBool(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
    m->table.set(vm->makeString("isfile"), 
                 vm->makeBool(!(fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)));
    
#else
    struct stat st;
    if (stat(path, &st) != 0)
        return vm->makeNil();
    
    Value map = vm->makeMap();
    MapInstance *m = map.asMap();
    
    m->table.set(vm->makeString("size").asString(), vm->makeInt((int)st.st_size));
    m->table.set(vm->makeString("isdir").asString(), vm->makeBool(S_ISDIR(st.st_mode)));
    m->table.set(vm->makeString("isfile").asString(), vm->makeBool(S_ISREG(st.st_mode)));
    m->table.set(vm->makeString("mode").asString(), vm->makeInt(st.st_mode));
    m->table.set(vm->makeString("mtime").asString(), vm->makeInt((int)st.st_mtime));
#endif
    
    return map;
}


void Interpreter::registerFS()
{
    addModule("fs")
        .addFunction("read", native_fs_read, 1)
        .addFunction("write", native_fs_write, 2)
        .addFunction("append", native_fs_append, 2)
        .addFunction("remove", native_fs_remove, 1)
        .addFunction("mkdir", native_fs_mkdir, 1)
        .addFunction("rmdir", native_fs_rmdir, 1)
        .addFunction("list", native_fs_list, 1)
        .addFunction("stat", native_fs_stat, 1);
}


#endif