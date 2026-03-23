# OS Module

```bulang
import os;
```

Operating system interaction, process management, and environment variables.

## Constants

| Constant | Type | Description |
|----------|------|-------------|
| `platform` | `string` | OS: `"linux"`, `"macos"`, `"windows"`, `"android"`, `"emscripten"`, `"unix"` |

## Process Management

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `spawn` | `cmd: string`, `...args: string` | `int` | Launch process, return PID |
| `spawn_shell` | `command: string` | `int` | Launch via shell, return PID |
| `spawn_capture` | `command: string` | `map` | Run via shell and capture output |
| `execute` | `command: string` | `int` | Run via system(), return exit code |
| `wait` | `pid: int`, `timeout_ms?: int` | `int\|nil` | Wait for process (nil on timeout) |
| `poll` | `pid: int` | `int\|nil` | Check status (non-blocking, nil if running) |
| `is_alive` | `pid: int` | `bool` | Check if process is running |
| `kill` | `pid: int`, `signal?: int` | `bool` | Terminate process (SIGTERM default) |

### spawn_capture Returns

```bulang
{
    output: string,  // Standard output
    stdout: string,  // Same as output
    code: int        // Exit code
}
```

## Environment

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `getenv` | `name: string` | `string\|nil` | Get environment variable |
| `setenv` | `name: string`, `value: string` | `bool` | Set environment variable |

## Working Directory

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `getcwd` | none | `string` | Get current working directory |
| `chdir` | `path: string` | `bool` | Change working directory |

## System

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `quit` | `code?: int` | never | Exit program with code (default 0) |

## Examples

```bulang
import os;

// Check platform
print("Running on: " + os.platform);
if (os.platform == "windows") {
    // Windows-specific code
}

// Environment variables
var home = os.getenv("HOME");
os.setenv("MY_VAR", "value");

// Working directory
print("Current: " + os.getcwd());
os.chdir("/tmp");

// Execute command (via system())
var code = os.execute("ls -la");
print("Exit code: " + str(code));

// Capture output
var result = os.spawn_capture("git status");
print("Code: " + str(result.code));
print("Output: " + result.output);

// Background process with args
var pid = os.spawn("sleep", "10");
print("Started PID: " + str(pid));

// Check if running
if (os.is_alive(pid)) {
    print("Process is running");
}

// Wait with timeout (5 seconds)
var exitCode = os.wait(pid, 5000);
if (exitCode == nil) {
    print("Timeout, killing...");
    os.kill(pid);
}

// Exit program
// os.quit(0);
```

## Shell Commands

```bulang
import os;

// Run shell command (handles pipes, redirects)
var pid = os.spawn_shell("ls -la | grep .txt > files.txt");
os.wait(pid);

// Cross-platform shell
if (os.platform == "windows") {
    os.execute("dir /b");
} else {
    os.execute("ls");
}
```

## Notes

- `spawn` returns immediately (async)
- `execute` blocks until complete  
- `spawn_capture` blocks and returns all output
- Use `spawn_shell` for complex commands with pipes/redirects
- `poll` returns nil if process is still running, exit code if finished
