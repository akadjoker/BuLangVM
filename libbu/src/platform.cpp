#include "platform.hpp"
#include <cstdio>
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

#endif // __EMSCRIPTEN__