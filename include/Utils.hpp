
#pragma once


#define CONSOLE_COLOR_RESET "\033[0m"
#define CONSOLE_COLOR_GREEN "\033[1;32m"
#define CONSOLE_COLOR_RED "\033[1;31m"
#define CONSOLE_COLOR_PURPLE "\033[1;35m"
#define CONSOLE_COLOR_CYAN "\033[0;36m"
#define CONSOLE_COLOR_YELLOW "\033[1;33m"
#define CONSOLE_COLOR_BLUE "\033[0;34m"

//void Log(int severity, const char *fmt, ...);
void Log(int severity, const char* fmt, ...);
void Log(int severity, const char* fmt, va_list args);

#define INFO(fmt, ...) Log(0, fmt, ##__VA_ARGS__)
#define WARNING(fmt, ...) Log(1, fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) Log(2, fmt, ##__VA_ARGS__)
#define PRINT(fmt, ...) Log(3, fmt, ##__VA_ARGS__)

char *LoadTextFile(const char *fileName);
void FreeTextFile(char *text);


static inline bool matchString(const char *str1, const char *str2,size_t bLen)
{
    size_t aLen = strlen(str1);
    
    if (aLen != bLen) return false;
    
    return (memcmp(str1, str2, aLen) == 0);
    
}

static inline double time_now()
{
    //  auto now = std::chrono::high_resolution_clock::now();
    // auto duration = now.time_since_epoch();
    // auto seconds = std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
    return (clock() / (double)CLOCKS_PER_SEC);
}