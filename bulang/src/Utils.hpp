
#pragma once
#include "Config.hpp"
#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"
#define CONSOLE_COLOR_CYAN "\033[0;36m"
#define CONSOLE_COLOR_YELLOW "\033[1;33m"
#define CONSOLE_COLOR_BLUE "\033[0;34m"

void Log(int severity, const char *fmt, ...);
void Log(int severity, const char *fmt, va_list args);

#define INFO(fmt, ...) Log(0, fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) Log(1, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) Log(2, fmt, ##__VA_ARGS__)
#define PRINT(fmt, ...) Log(3, fmt, ##__VA_ARGS__)

char *LoadTextFile(const char *fileName);
void FreeTextFile(char *text);
bool matchString(const char *str1, const char *str2, size_t bLen);
double time_now();

u32 GROW_CAPACITY(u32 capacity);
const char *FormatText(const char *text, ...);
