
#include "interpreter.hpp"


#ifdef BU_ENABLE_FILE_IO

#include "platform.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

// ============================================
// FILE MODULE - COMPLETO
// ============================================

#include <cstring>
#include <vector>
#include <string>

enum class FileMode {
    READ,      
    WRITE,     
    READ_WRITE 
};

struct FileBuffer {
    std::vector<uint8_t> data;
    size_t cursor;
    std::string path;
    FileMode mode;
    bool modified;
};

static std::vector<FileBuffer*> openFiles;
static int nextFileId = 1;

// ============================================
// CLEANUP
// ============================================

static void FileModuleCleanup()
{
    for (auto fb : openFiles)
    {
        if (fb)
        {
            if (fb->modified && fb->mode != FileMode::READ)
                OsFileWrite(fb->path.c_str(), fb->data.data(), fb->data.size());
            delete fb;
        }
    }
    openFiles.clear();
}

// ============================================
// OPEN
// ============================================

Value native_file_open(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("file.open expects (path, mode?)");
        return vm->makeNil();
    }
    
    const char *path = args[0].asStringChars();
    const char *modeStr = "r";
    
    if (argCount >= 2 && args[1].isString())
        modeStr = args[1].asStringChars();
    
    FileMode mode;
    if (strcmp(modeStr, "r") == 0)
        mode = FileMode::READ;
    else if (strcmp(modeStr, "w") == 0)
        mode = FileMode::WRITE;
    else if (strcmp(modeStr, "rw") == 0)
        mode = FileMode::READ_WRITE;
    else
    {
        vm->runtimeError("Invalid mode '%s'. Use 'r', 'w', or 'rw'", modeStr);
        return vm->makeNil();
    }
    
    FileBuffer *fb = new FileBuffer();
    fb->path = path;
    fb->cursor = 0;
    fb->mode = mode;
    fb->modified = false;
    
    if (mode == FileMode::READ || mode == FileMode::READ_WRITE)
    {
        int fileSize = OsFileSize(path);
        
        if (fileSize > 0)
        {
            fb->data.resize(fileSize);
            int bytesRead = OsFileRead(path, fb->data.data(), fileSize);
            
            if (bytesRead < 0)
            {
                delete fb;
                vm->runtimeError("Failed to read file '%s'", path);
                return vm->makeNil();
            }
            
            fb->data.resize(bytesRead);
        }
        else if (mode == FileMode::READ)
        {
            delete fb;
            vm->runtimeError("File '%s' does not exist", path);
            return vm->makeNil();
        }
    }
    
    openFiles.push_back(fb);
    return vm->makeInt(nextFileId++);
}

// ============================================
// SAVE
// ============================================

Value native_file_save(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot save file opened in read mode");
        return vm->makeBool(false);
    }
    
    int written = OsFileWrite(fb->path.c_str(), fb->data.data(), fb->data.size());
    if (written < 0)
        return vm->makeBool(false);
    
    fb->modified = false;
    return vm->makeBool(true);
}

// ============================================
// CLOSE
// ============================================

Value native_file_close(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->modified && fb->mode != FileMode::READ)
    {
        OsFileWrite(fb->path.c_str(), fb->data.data(), fb->data.size());
    }
    
    delete fb;
    openFiles[id-1] = nullptr;
    
    return vm->makeBool(true);
}

// ============================================
// WRITE BYTE
// ============================================

Value native_file_write_byte(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot write to file opened in read mode");
        return vm->makeBool(false);
    }
    
    if (fb->cursor >= fb->data.size())
        fb->data.resize(fb->cursor + 1);
    
    fb->data[fb->cursor++] = (uint8_t)args[1].asInt();
    fb->modified = true;
    
    return vm->makeBool(true);
}

// ============================================
// WRITE INT
// ============================================

Value native_file_write_int(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot write to file opened in read mode");
        return vm->makeBool(false);
    }
    
    if (fb->cursor + sizeof(int32_t) > fb->data.size())
        fb->data.resize(fb->cursor + sizeof(int32_t));
    
    int32_t value = args[1].asInt();
    memcpy(fb->data.data() + fb->cursor, &value, sizeof(int32_t));
    fb->cursor += sizeof(int32_t);
    fb->modified = true;
    
    return vm->makeBool(true);
}

// ============================================
// WRITE FLOAT
// ============================================

Value native_file_write_float(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot write to file opened in read mode");
        return vm->makeBool(false);
    }
    
    if (fb->cursor + sizeof(float) > fb->data.size())
        fb->data.resize(fb->cursor + sizeof(float));
    
    float value = (float)args[1].asNumber();
    memcpy(fb->data.data() + fb->cursor, &value, sizeof(float));
    fb->cursor += sizeof(float);
    fb->modified = true;
    
    return vm->makeBool(true);
}

// ============================================
// WRITE DOUBLE
// ============================================

Value native_file_write_double(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot write to file opened in read mode");
        return vm->makeBool(false);
    }
    
    if (fb->cursor + sizeof(double) > fb->data.size())
        fb->data.resize(fb->cursor + sizeof(double));
    
    double value = args[1].asNumber();
    memcpy(fb->data.data() + fb->cursor, &value, sizeof(double));
    fb->cursor += sizeof(double);
    fb->modified = true;
    
    return vm->makeBool(true);
}

// ============================================
// WRITE BOOL
// ============================================

Value native_file_write_bool(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isBool())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot write to file opened in read mode");
        return vm->makeBool(false);
    }
    
    if (fb->cursor >= fb->data.size())
        fb->data.resize(fb->cursor + 1);
    
    fb->data[fb->cursor++] = args[1].asBool() ? 1 : 0;
    fb->modified = true;
    
    return vm->makeBool(true);
}

// ============================================
// WRITE STRING
// ============================================

Value native_file_write_string(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isString())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->mode == FileMode::READ)
    {
        vm->runtimeError("Cannot write to file opened in read mode");
        return vm->makeBool(false);
    }
    
    const char *str = args[1].asStringChars();
    int len = args[1].asString()->length();
    
    size_t needed = sizeof(int32_t) + len;
    if (fb->cursor + needed > fb->data.size())
        fb->data.resize(fb->cursor + needed);
    
    int32_t size = len;
    memcpy(fb->data.data() + fb->cursor, &size, sizeof(int32_t));
    fb->cursor += sizeof(int32_t);
    
    memcpy(fb->data.data() + fb->cursor, str, len);
    fb->cursor += len;
    fb->modified = true;
    
    return vm->makeBool(true);
}

// ============================================
// READ BYTE
// ============================================

Value native_file_read_byte(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->cursor >= fb->data.size())
        return vm->makeNil();
    
    uint8_t value = fb->data[fb->cursor++];
    return vm->makeInt(value);
}

// ============================================
// READ INT
// ============================================

Value native_file_read_int(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->cursor + sizeof(int32_t) > fb->data.size())
        return vm->makeNil();
    
    int32_t value;
    memcpy(&value, fb->data.data() + fb->cursor, sizeof(int32_t));
    fb->cursor += sizeof(int32_t);
    
    return vm->makeInt(value);
}

// ============================================
// READ FLOAT
// ============================================

Value native_file_read_float(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->cursor + sizeof(float) > fb->data.size())
        return vm->makeNil();
    
    float value;
    memcpy(&value, fb->data.data() + fb->cursor, sizeof(float));
    fb->cursor += sizeof(float);
    
    return vm->makeDouble(value);
}

// ============================================
// READ DOUBLE
// ============================================

Value native_file_read_double(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->cursor + sizeof(double) > fb->data.size())
        return vm->makeNil();
    
    double value;
    memcpy(&value, fb->data.data() + fb->cursor, sizeof(double));
    fb->cursor += sizeof(double);
    
    return vm->makeDouble(value);
}

// ============================================
// READ BOOL
// ============================================

Value native_file_read_bool(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->cursor >= fb->data.size())
        return vm->makeNil();
    
    uint8_t value = fb->data[fb->cursor++];
    return vm->makeBool(value != 0);
}

// ============================================
// READ STRING
// ============================================

Value native_file_read_string(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    
    if (fb->cursor + sizeof(int32_t) > fb->data.size())
        return vm->makeNil();
    
    int32_t len;
    memcpy(&len, fb->data.data() + fb->cursor, sizeof(int32_t));
    fb->cursor += sizeof(int32_t);
    
    if (len < 0 || fb->cursor + len > fb->data.size())
        return vm->makeNil();
    
    std::string str((char*)(fb->data.data() + fb->cursor), len);
    fb->cursor += len;
    
    return vm->makeString(str.c_str());
}

// ============================================
// SEEK
// ============================================

Value native_file_seek(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    int pos = args[1].asInt();
    
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeBool(false);
    
    FileBuffer *fb = openFiles[id-1];
    
    if (pos < 0 || pos > (int)fb->data.size())
        return vm->makeBool(false);
    
    fb->cursor = pos;
    return vm->makeBool(true);
}

// ============================================
// TELL
// ============================================

Value native_file_tell(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    return vm->makeInt((int)fb->cursor);
}

// ============================================
// SIZE
// ============================================

Value native_file_size(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();
    
    int id = args[0].asInt();
    if (id <= 0 || id > openFiles.size() || !openFiles[id-1])
        return vm->makeNil();
    
    FileBuffer *fb = openFiles[id-1];
    return vm->makeInt((int)fb->data.size());
}

// ============================================
// REGISTO
// ============================================

void Interpreter::registerFile()
{
    static bool initialized = false;
    if (!initialized)
    {
        atexit(FileModuleCleanup);
        initialized = true;
    }
    
    addModule("file")
        .addFunction("open", native_file_open, -1)
        .addFunction("save", native_file_save, 1)
        .addFunction("close", native_file_close, 1)
        
        .addFunction("write_byte", native_file_write_byte, 2)
        .addFunction("write_int", native_file_write_int, 2)
        .addFunction("write_float", native_file_write_float, 2)
        .addFunction("write_double", native_file_write_double, 2)
        .addFunction("write_bool", native_file_write_bool, 2)
        .addFunction("write_string", native_file_write_string, 2)
        
        .addFunction("read_byte", native_file_read_byte, 1)
        .addFunction("read_int", native_file_read_int, 1)
        .addFunction("read_float", native_file_read_float, 1)
        .addFunction("read_double", native_file_read_double, 1)
        .addFunction("read_bool", native_file_read_bool, 1)
        .addFunction("read_string", native_file_read_string, 1)
        
        .addFunction("seek", native_file_seek, 2)
        .addFunction("tell", native_file_tell, 1)
        .addFunction("size", native_file_size, 1);
}

#endif