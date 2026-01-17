
#include "interpreter.hpp"
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

Value native_os_execute(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeInt(-1);

    int result = system(args[0].asStringChars());
    return vm->makeInt(result);
}

Value native_os_getenv(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeNil();

    const char *value = getenv(args[0].asStringChars());
    return value ? vm->makeString(value) : vm->makeNil();
}

Value native_os_setenv(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString() || !args[1].isString())
        return vm->makeBool(false);

#ifdef _WIN32
    return vm->makeBool(_putenv_s(args[0].asStringChars(),
                                  args[1].asStringChars()) == 0);
#else
    return vm->makeBool(setenv(args[0].asStringChars(),
                               args[1].asStringChars(), 1) == 0);
#endif
}

Value native_os_getcwd(Interpreter *vm, int argCount, Value *args)
{
    char buffer[4096];
    if (getcwd(buffer, sizeof(buffer)))
        return vm->makeString(buffer);
    return vm->makeNil();
}

Value native_os_chdir(Interpreter *vm, int argCount, Value *args)
{
    if (!args[0].isString())
        return vm->makeBool(false);

    return vm->makeBool(chdir(args[0].asStringChars()) == 0);
}

Value native_os_exit(Interpreter *vm, int argCount, Value *args)
{
    int code = args[0].isInt() ? args[0].asInt() : 0;
    exit(code);
    return vm->makeNil();
}

// ============================================
// OS.SPAWN - Versão Completa
// ============================================

Value native_os_spawn(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("os.spawn expects at least command string");
        return vm->makeInt(-1);
    }

    const char *command = args[0].asStringChars();

    // Valida que o comando existe
    if (!command || strlen(command) == 0)
    {
        vm->runtimeError("os.spawn: empty command");
        return vm->makeInt(-1);
    }

#ifdef _WIN32
    STARTUPINFOA si = {0};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

    // Constrói command line com proteção de aspas
    std::string cmdLine;
    cmdLine.reserve(1024);

    // Comando principal sempre entre aspas
    cmdLine += "\"";
    cmdLine += command;
    cmdLine += "\"";

    // Adiciona argumentos
    for (int i = 1; i < argCount; i++)
    {
        if (!args[i].isString())
            continue;

        const char *arg = args[i].asStringChars();
        cmdLine += " \"";

        // Escapa aspas internas
        for (const char *p = arg; *p; p++)
        {
            if (*p == '"')
                cmdLine += "\\\"";
            else
                cmdLine += *p;
        }

        cmdLine += "\"";
    }

    // Usa vector para buffer mutável
    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back(0);

    BOOL success = CreateProcessA(
        NULL,             // lpApplicationName
        cmdBuf.data(),    // lpCommandLine
        NULL,             // lpProcessAttributes
        NULL,             // lpThreadAttributes
        FALSE,            // bInheritHandles
        CREATE_NO_WINDOW, // dwCreationFlags
        NULL,             // lpEnvironment
        NULL,             // lpCurrentDirectory
        &si,              // lpStartupInfo
        &pi               // lpProcessInformation
    );

    if (!success)
    {
        DWORD error = GetLastError();
        vm->runtimeError("os.spawn failed: error code %lu", error);
        return vm->makeInt(-1);
    }

    DWORD pid = pi.dwProcessId;

    // Fecha handles (não precisamos deles)
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return vm->makeInt((int)pid);

#else
    // Unix: fork + execvp
    pid_t pid = fork();

    if (pid == -1)
    {
        vm->runtimeError("os.spawn: fork failed");
        return vm->makeInt(-1);
    }
    else if (pid == 0)
    {
        // Processo filho
        std::vector<char *> argv;
        argv.reserve(argCount + 1);

        // Adiciona comando
        argv.push_back((char *)command);

        // Adiciona argumentos
        for (int i = 1; i < argCount; i++)
        {
            if (args[i].isString())
                argv.push_back((char *)args[i].asStringChars());
        }

        argv.push_back(NULL);

        // Executa
        execvp(command, argv.data());

        // Se chegou aqui, execvp falhou
        perror("execvp");
        _exit(127);
    }
    else
    {
        // Processo pai - retorna PID
        return vm->makeInt((int)pid);
    }
#endif
}

// ============================================
// OS.SPAWN_SHELL - Executa via shell
// ============================================

Value native_os_spawn_shell(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("os.spawn_shell expects command string");
        return vm->makeInt(-1);
    }

    const char *command = args[0].asStringChars();

#ifdef _WIN32
    STARTUPINFOA si = {0};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {0};

    // Usa cmd.exe /C
    std::string cmdLine = "cmd.exe /C \"";
    cmdLine += command;
    cmdLine += "\"";

    std::vector<char> cmdBuf(cmdLine.begin(), cmdLine.end());
    cmdBuf.push_back(0);

    if (!CreateProcessA(NULL, cmdBuf.data(), NULL, NULL, FALSE,
                        CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        return vm->makeInt(-1);
    }

    DWORD pid = pi.dwProcessId;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return vm->makeInt((int)pid);

#else
    pid_t pid = fork();

    if (pid == -1)
        return vm->makeInt(-1);
    else if (pid == 0)
    {
        // Usa sh -c
        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(127);
    }
    else
    {
        return vm->makeInt((int)pid);
    }
#endif
}

// ============================================
// OS.SPAWN_CAPTURE - Executa e captura output
// ============================================

Value native_os_spawn_capture(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isString())
    {
        vm->runtimeError("os.spawn_capture expects command");
        return vm->makeNil();
    }

    const char *command = args[0].asStringChars();

#ifdef _WIN32
    // Windows: usa popen
    FILE *pipe = _popen(command, "r");
    if (!pipe)
        return vm->makeNil();

    std::string output;
    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        output += buffer;
    }

    int exitCode = _pclose(pipe);

    // Retorna map com {output: string, code: int}
    Value result = vm->makeMap();
    MapInstance *map = result.asMap();

    map->table.set(vm->makeString("output"), vm->makeString(output.c_str()));
    map->table.set(vm->makeString("code"), vm->makeInt(exitCode));

    return result;

#else
    FILE *pipe = popen(command, "r");
    if (!pipe)
        return vm->makeNil();

    std::string output;
    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), pipe))
    {
        output += buffer;
    }

    int status = pclose(pipe);
    int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

    

    Value result = vm->makeMap();
    MapInstance *map = result.asMap();

    map->table.set(vm->makeString("output").asString(), vm->makeString(output.c_str()));
    map->table.set(vm->makeString("code").asString(), vm->makeInt(exitCode));
    map->table.set(vm->makeString("status").asString(), vm->makeInt(status));

    return result;
#endif
}

// ============================================
// OS.KILL - Termina processo
// ============================================

Value native_os_kill(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
    {
        vm->runtimeError("os.kill expects process ID");
        return vm->makeBool(false);
    }

    int pid = args[0].asInt();

#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!hProcess)
        return vm->makeBool(false);

    BOOL result = TerminateProcess(hProcess, 1);
    CloseHandle(hProcess);

    return vm->makeBool(result != 0);

#else
    return vm->makeBool(kill(pid, SIGTERM) == 0);
#endif
}

Value native_os_wait(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeInt(-1);

    int pid = args[0].asInt();

#ifdef _WIN32
    HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION,
                                  FALSE, pid);
    if (!hProcess)
        return vm->makeInt(-1);

    WaitForSingleObject(hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);

    return vm->makeInt((int)exitCode);
#else
    int status;
    if (waitpid(pid, &status, 0) == -1)
        return vm->makeInt(-1);

    if (WIFEXITED(status))
        return vm->makeInt(WEXITSTATUS(status));

    return vm->makeInt(-1);
#endif
}

void Interpreter::registerOS()
{
#ifdef _WIN32
    const char *platform = "windows";
#elif defined(__APPLE__)
    const char *platform = "macos";
#elif defined(__linux__)
    const char *platform = "linux";
#elif defined(__unix__)
    const char *platform = "unix";
#elif defined(__ANDROID__)
    const char *platform = "android";
#else
    const char *platform = "unknown";
#endif

    addModule("os")
        .addString("platform", platform)
        .addFunction("spawn", native_os_spawn, -1)                // Spawn processo
        .addFunction("spawn_shell", native_os_spawn_shell, 1)     // Via shell
        .addFunction("spawn_capture", native_os_spawn_capture, 1) // Captura output
        .addFunction("wait", native_os_wait, 1)                   // Espera processo
        .addFunction("kill", native_os_kill, 1)                   // Termina processo
        .addFunction("execute", native_os_execute, 1) 
        .addFunction("getenv", native_os_getenv, 1)
        .addFunction("setenv", native_os_setenv, 2)
        .addFunction("getcwd", native_os_getcwd, 0)
        .addFunction("chdir", native_os_chdir, 1)
        .addFunction("quit", native_os_exit, 1);
}
