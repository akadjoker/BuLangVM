#ifndef PLATFORM_H
#define PLATFORM_H

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

#ifdef __cplusplus
}
#endif

#endif // PLATFORM_H