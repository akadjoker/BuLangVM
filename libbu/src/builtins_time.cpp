// ============================================
// TIME MODULE - Cross-platform
// ============================================
#include "interpreter.hpp"
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>

// ============================================
// TIME.NOW - Timestamp atual (segundos desde epoch)
// ============================================

Value native_time_now(Interpreter *vm, int argCount, Value *args)
{
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();
    
    return vm->makeInt((int)seconds);
}

// ============================================
// TIME.NOW_MS - Timestamp em milissegundos
// ============================================

Value native_time_now_ms(Interpreter *vm, int argCount, Value *args)
{
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    
    return vm->makeInt((int)ms);
}

// ============================================
// TIME.SLEEP - Pausa execução (segundos)
// ============================================

Value native_time_sleep(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1)
    {
        vm->runtimeError("time.sleep expects at least 1 argument");
        return vm->makeNil();
    }
    
    double seconds = args[0].isInt() 
        ? (double)args[0].asInt() 
        : args[0].asDouble();
    
    if (seconds < 0)
        return vm->makeNil();
    
    auto duration = std::chrono::duration<double>(seconds);
    std::this_thread::sleep_for(duration);
    
    return vm->makeNil();
}

// ============================================
// TIME.SLEEP_MS - Pausa em milissegundos
// ============================================

Value native_time_sleep_ms(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
    {
        vm->runtimeError("time.sleep_ms expects integer milliseconds");
        return vm->makeNil();
    }
    
    int ms = args[0].asInt();
    if (ms < 0)
        return vm->makeNil();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    
    return vm->makeNil();
}

// ============================================
// TIME.CLOCK - Tempo de CPU (high precision)
// ============================================

Value native_time_clock(Interpreter *vm, int argCount, Value *args)
{
    auto now = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()
    ).count();
    
    // Retorna em segundos (double para precisão)
    return vm->makeDouble((double)ns / 1e9);
}

// ============================================
// TIME.DATE - Decompõe timestamp em componentes
// ============================================

Value native_time_date(Interpreter *vm, int argCount, Value *args)
{
    time_t timestamp;
    
    if (argCount == 0)
    {
        timestamp = time(nullptr);
    }
    else if (args[0].isInt())
    {
        timestamp = (time_t)args[0].asInt();
    }
    else
    {
        vm->runtimeError("time.date expects optional integer timestamp");
        return vm->makeNil();
    }
    
    struct tm *timeinfo = localtime(&timestamp);
    
    if (!timeinfo)
        return vm->makeNil();
    
    // Retorna map com componentes
    Value result = vm->makeMap();
    MapInstance *map = result.asMap();
    
    map->table.set(vm->makeString("year").asString(), vm->makeInt(timeinfo->tm_year + 1900));
    map->table.set(vm->makeString("month").asString(), vm->makeInt(timeinfo->tm_mon + 1));
    map->table.set(vm->makeString("day").asString(), vm->makeInt(timeinfo->tm_mday));
    map->table.set(vm->makeString("hour").asString(), vm->makeInt(timeinfo->tm_hour));
    map->table.set(vm->makeString("minute").asString(), vm->makeInt(timeinfo->tm_min));
    map->table.set(vm->makeString("second").asString(), vm->makeInt(timeinfo->tm_sec));
    map->table.set(vm->makeString("weekday").asString(), vm->makeInt(timeinfo->tm_wday));
    map->table.set(vm->makeString("yearday").asString(), vm->makeInt(timeinfo->tm_yday));
    
    return result;
}

// ============================================
// TIME.FORMAT - Formata timestamp
// ============================================

Value native_time_format(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1)
    {
        vm->runtimeError("time.format expects at least timestamp");
        return vm->makeNil();
    }
    
    time_t timestamp;
    const char *format = "%Y-%m-%d %H:%M:%S";  // Default ISO format
    
    // Primeiro argumento: timestamp
    if (args[0].isInt())
    {
        timestamp = (time_t)args[0].asInt();
    }
    else
    {
        timestamp = time(nullptr);
    }
    
    // Segundo argumento opcional: formato
    if (argCount >= 2 && args[1].isString())
    {
        format = args[1].asStringChars();
    }
    
    struct tm *timeinfo = localtime(&timestamp);
    if (!timeinfo)
        return vm->makeNil();
    
    char buffer[256];
    size_t len = strftime(buffer, sizeof(buffer), format, timeinfo);
    
    if (len == 0)
        return vm->makeNil();
    
    return vm->makeString(buffer);
}

// ============================================
// TIME.PARSE - Parse string para timestamp
// ============================================

Value native_time_parse(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isString() || !args[1].isString())
    {
        vm->runtimeError("time.parse expects (dateString, formatString)");
        return vm->makeNil();
    }
    
    const char *dateStr = args[0].asStringChars();
    const char *formatStr = args[1].asStringChars();
    
    struct tm timeinfo = {0};
    
#ifdef _WIN32
    // Windows não tem strptime, usa sscanf básico
    // Formato simplificado: "YYYY-MM-DD HH:MM:SS"
    if (sscanf(dateStr, "%d-%d-%d %d:%d:%d",
               &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
               &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) >= 3)
    {
        timeinfo.tm_year -= 1900;
        timeinfo.tm_mon -= 1;
        time_t timestamp = mktime(&timeinfo);
        return vm->makeInt((int)timestamp);
    }
#else
    if (strptime(dateStr, formatStr, &timeinfo) != nullptr)
    {
        time_t timestamp = mktime(&timeinfo);
        return vm->makeInt((int)timestamp);
    }
#endif
    
    return vm->makeNil();
}

// ============================================
// TIME.DIFF - Diferença entre timestamps
// ============================================

Value native_time_diff(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isInt())
    {
        vm->runtimeError("time.diff expects two timestamps");
        return vm->makeNil();
    }
    
    int t1 = args[0].asInt();
    int t2 = args[1].asInt();
    
    return vm->makeInt(t1 - t2);
}

// ============================================
// Registo do módulo
// ============================================

void Interpreter::registerTime()
{
    addModule("time")
        // Timestamps
        .addFunction("now", native_time_now, 0)           // Segundos
        .addFunction("now_ms", native_time_now_ms, 0)     // Milissegundos
        .addFunction("current", native_time_clock, 0)       // High precision
        
        // Sleep
        .addFunction("sleep", native_time_sleep, 1)       // Segundos
        .addFunction("sleep_ms", native_time_sleep_ms, 1) // Milissegundos
        
        // Date/Time manipulation
        .addFunction("date", native_time_date, -1)        // Decompõe timestamp
        .addFunction("ftime", native_time_format, -1)    // Formata
        .addFunction("parse", native_time_parse, 2)       // Parse string
        .addFunction("diff", native_time_diff, 2);        // Diferença
}