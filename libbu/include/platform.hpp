#ifndef PLATFORM_H
#define PLATFORM_H
#include <cstdio>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Platform-specific printf
 * Linux/Desktop: prints to stdout
 * WASM/Web: captures to OutputCapture
 */
void OsPrintf(const char* fmt, ...);

/**
 * Platform-specific error printf
 * Linux/Desktop: prints to stderr
 * WASM/Web: captures to OutputCapture with error prefix
 */
void OsEPrintf(const char* fmt, ...);


int OsFileWrite(const char *filename, const void *data, size_t size);
int OsFileRead(const char *filename, void *buffer, size_t maxSize);
bool OsFileExists(const char *filename);
int OsFileSize(const char *filename);
bool OsFileDelete(const char *filename);


#ifdef __cplusplus
}
#endif

#endif // PLATFORM_H