#include "platform.hpp"

#include <cstdarg>

// ============================================
// LINUX/DESKTOP Platform
// ============================================
#ifdef __linux__

void OsPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}

void OsEPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

#endif // __linux__

// ============================================
// WINDOWS Platform
// ============================================
#ifdef _WIN32

void OsPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}

void OsEPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

#endif // _WIN32

// ============================================
// MAC/APPLE Platform
// ============================================
#ifdef __APPLE__

void OsPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}

void OsEPrintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    fflush(stderr);
}

#endif // __APPLE__

// ============================================
// WASM/EMSCRIPTEN Platform (WEB/PLAYGROUND)
// ============================================
#ifdef __EMSCRIPTEN__

#include <string>
#include <emscripten.h>
#include "Outputcapture.h"

extern OutputCapture *g_currentOutput;

void OsPrintf(const char *fmt, ...)
{
    char buffer[4096];

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (len > 0)
    {
        if (g_currentOutput)
        {
            //  Playground mode - capture to OutputCapture
            g_currentOutput->write(std::string(buffer, len));
        }
        else
        {
            //  Fallback - direct printf
            printf("%s", buffer);
        }
    }
}

void OsEPrintf(const char *fmt, ...)
{
    char buffer[4096];

    va_list args;
    va_start(args, fmt);

    // Add error prefix
    int offset = snprintf(buffer, sizeof(buffer), "❌ ERROR: ");
    if (offset < 0)
        offset = 0;

    int len = vsnprintf(buffer + offset, sizeof(buffer) - offset, fmt, args);
    va_end(args);

    if (len > 0)
    {
        if (g_currentOutput)
        {

            g_currentOutput->write(std::string(buffer, offset + len));
        }
        else
        {
            //  Fallback
            printf("%s", buffer);
        }
    }
}

// Emscripten tem filesystem virtual via IDBFS
int OsFileWrite(const char *filename, const void *data, size_t size)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        OsEPrintf("Failed to open file '%s' for writing", filename);
        return -1;
    }

    size_t written = fwrite(data, 1, size, file);
    fclose(file);

    // Sync filesystem
    EM_ASM(
        FS.syncfs(false, function(err) {
            if (err) console.error('FS sync error:', err); }););

    if (written != size)
    {
        return -1;
    }

    return (int)written;
}

int OsFileRead(const char *filename, void *buffer, size_t maxSize)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        OsEPrintf("Failed to open file '%s' for reading", filename);
        return -1;
    }

    if (buffer == NULL)
    {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        return (int)size;
    }

    size_t read = fread(buffer, 1, maxSize, file);
    fclose(file);

    return (int)read;
}

bool OsFileExists(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

int OsFileSize(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return (int)size;
}

bool OsFileDelete(const char *filename)
{
    bool result = remove(filename) == 0;

    // Sync filesystem
    EM_ASM(
        FS.syncfs(false, function(err) {
            if (err) console.error('FS sync error:', err); }););

    return result;
}

#endif // __EMSCRIPTEN__

// ============================================
// LINUX/DESKTOP/WINDOWS/MAC
// ============================================
#if defined(__linux__) || defined(_WIN32) || defined(__APPLE__)

int OsFileWrite(const char *filename, const void *data, size_t size)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        OsEPrintf("Failed to open file '%s' for writing", filename);
        return -1;
    }

    size_t written = fwrite(data, 1, size, file);
    fclose(file);

    if (written != size)
    {
        OsEPrintf("Failed to write %zu bytes (wrote %zu)", size, written);
        return -1;
    }

    return (int)written;
}

int OsFileRead(const char *filename, void *buffer, size_t maxSize)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        OsEPrintf("Failed to open file '%s' for reading", filename);
        return -1;
    }

 
    if (buffer == NULL)
    {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        return (int)size;
    }

    size_t read = fread(buffer, 1, maxSize, file);
    fclose(file);

    return (int)read;
}

bool OsFileExists(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file)
    {
        fclose(file);
        return true;
    }
    return false;
}

int OsFileSize(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
        return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return (int)size;
}

bool OsFileDelete(const char *filename)
{
    return remove(filename) == 0;
}

#endif
