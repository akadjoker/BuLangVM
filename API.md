# 📚 BuLang API Reference

Complete documentation of all built-in modules and functions.

---

## 📋 Table of Contents

- [Core Functions](#-core-functions)
- [Math Module](#-math-module)
- [Time Module](#-time-module)
- [File Module](#-file-module)
- [FileSystem (fs) Module](#-filesystem-fs-module)
- [Path Module](#-path-module)
- [OS Module](#-os-module)
- [Socket Module](#-socket-module)

---

## 🔧 Core Functions

Built-in functions available globally without imports.

### `format(template, ...args)`
Format string with `{}` placeholders.

```bulang
msg = format("Hello, {}! You have {} messages.", "Alice", 5);
# "Hello, Alice! You have 5 messages."
```

### `write(template, ...args)`
Print formatted string without newline.

```bulang
write("Loading {}%", progress);
```

### `print_stack([message])`
Debug function to print current VM stack state.

```bulang
print_stack("Debug checkpoint");
```

### `_gc()`
Manually trigger garbage collection.

```bulang
_gc();  # Force GC cycle
```

---

## 🔢 Math Module

Mathematical functions and constants.

### Constants

```bulang
import math;

math.PI        # 3.14159265358979
math.E         # 2.71828182845905
math.TAU       # 6.28318530717958 (2π)
math.SQRT2     # 1.41421356
math.MIN_INT   # -2147483648
math.MAX_INT   # 2147483647
```

### Basic Functions

#### `min(a, b)` / `max(a, b)`
Return minimum or maximum value.

```bulang
smallest = math.min(10, 20);  # 10
largest = math.max(10, 20);   # 20
```

#### `clamp(value, min, max)`
Constrain value to range.

```bulang
health = math.clamp(health, 0, 100);
```

### Random Numbers

#### `seed(value)`
Set random number generator seed for deterministic results.

```bulang
math.seed(12345);  # Reproducible random
```

#### `rand()` / `rand(max)` / `rand(min, max)`
Generate random float [0.0, 1.0] or within range.

```bulang
random = math.rand();           # 0.0 to 1.0
random = math.rand(100);        # 0.0 to 100.0
random = math.rand(50, 100);    # 50.0 to 100.0
```

#### `irand()` / `irand(max)` / `irand(min, max)`
Generate random integer.

```bulang
dice = math.irand(1, 6);        # 1 to 6
index = math.irand(0, len(arr)-1);
```

### Interpolation & Ranges

#### `lerp(start, end, t)`
Linear interpolation between two values.

```bulang
pos = math.lerp(0, 100, 0.5);   # 50.0
color = math.lerp(0, 255, alpha);
```

#### `map(value, in_min, in_max, out_min, out_max)`
Remap value from one range to another.

```bulang
# Map 0-1023 (sensor) to 0-100 (percentage)
percent = math.map(sensor, 0, 1023, 0, 100);
```

#### `sign(x)`
Return -1, 0, or 1 based on sign.

```bulang
direction = math.sign(velocity);  # -1, 0, or 1
```

### Geometry & Distance

#### `hypot(dx, dy)`
Calculate hypotenuse (safe from overflow).

```bulang
distance = math.hypot(x2 - x1, y2 - y1);
```

### Logarithms

#### `log10(x)` / `log2(x)`
Logarithm base 10 or base 2.

```bulang
decibels = 20 * math.log10(amplitude);
bits = math.log2(value);
```

### Hyperbolic Functions

#### `sinh(x)` / `cosh(x)` / `tanh(x)`
Hyperbolic sine, cosine, tangent.

```bulang
result = math.sinh(value);
```

### Smooth Transitions

#### `smoothstep(t)` / `smoothstep(edge0, edge1, x)`
Smooth hermite interpolation.

```bulang
# Normalize t (0..1)
smooth = math.smoothstep(t);

# With custom range
smooth = math.smoothstep(0.2, 0.8, x);
```

#### `smootherstep(t)` / `smootherstep(edge0, edge1, x)`
Even smoother Ken Perlin interpolation.

```bulang
ultra_smooth = math.smootherstep(t);
```

#### `hermite(value1, tangent1, value2, tangent2, amount)`
Hermite curve interpolation.

```bulang
pos = math.hermite(p0, t0, p1, t1, time);
```

### Utility Functions

#### `repeat(t, length)`
Wrap value within range [0, length].

```bulang
# Loop animation from 0 to 10
frame = math.repeat(time, 10);
```

#### `ping_pong(t, length)`
Ping-pong value back and forth.

```bulang
# Oscillate between 0 and 5
wave = math.ping_pong(time, 5);
```

---

## ⏱️ Time Module

Time management and scheduling functions.

```bulang
import time;
```

### Timestamps

#### `now()`
Current Unix timestamp (seconds since epoch).

```bulang
timestamp = time.now();  # e.g., 1705593600
```

#### `now_ms()`
Current timestamp in milliseconds.

```bulang
ms = time.now_ms();  # e.g., 1705593600000
```

#### `current()`
High-precision clock (nanosecond resolution).

```bulang
start = time.current();
# ... do work ...
elapsed = time.current() - start;
```

### Sleep Functions

#### `sleep(seconds)`
Pause execution for seconds.

```bulang
time.sleep(2);      # Wait 2 seconds
time.sleep(0.5);    # Wait 500ms
```

#### `sleep_ms(milliseconds)`
Pause execution for milliseconds.

```bulang
time.sleep_ms(100);  # Wait 100ms
```

### Date/Time Manipulation

#### `date([timestamp])`
Decompose timestamp into components.

```bulang
d = time.date();  # Current time
# Returns map with: year, month, day, hour, minute, second, weekday, yearday

print(d["year"]);    # 2025
print(d["month"]);   # 1-12
print(d["day"]);     # 1-31
print(d["weekday"]); # 0-6 (Sunday=0)
```

#### `ftime(timestamp, [format])`
Format timestamp as string.

```bulang
# Default ISO format
str = time.ftime(time.now());
# "2025-01-18 14:30:00"

# Custom format
str = time.ftime(time.now(), "%Y/%m/%d %H:%M");
# "2025/01/18 14:30"
```

Format codes:
- `%Y` - Year (4 digits)
- `%m` - Month (01-12)
- `%d` - Day (01-31)
- `%H` - Hour (00-23)
- `%M` - Minute (00-59)
- `%S` - Second (00-59)

#### `parse(dateString, formatString)`
Parse string to timestamp.

```bulang
ts = time.parse("2025-01-18 14:30:00", "%Y-%m-%d %H:%M:%S");
```

#### `diff(timestamp1, timestamp2)`
Calculate difference between timestamps.

```bulang
elapsed = time.diff(end_time, start_time);
print("Took {} seconds", elapsed);
```

---

## 📄 File Module

Binary file I/O with cursor-based reading/writing.

```bulang
import file;
```

### File Operations

#### `open(path, [mode])`
Open file for reading/writing. Returns file ID.

```bulang
f = file.open("data.bin", "rw");  # Read-write
f = file.open("save.dat", "w");   # Write only
f = file.open("load.dat", "r");   # Read only
```

Modes:
- `"r"` - Read only
- `"w"` - Write only (creates new)
- `"rw"` - Read-write

#### `save(fileId)`
Save changes to disk.

```bulang
file.save(f);
```

#### `close(fileId)`
Close file (auto-saves if modified).

```bulang
file.close(f);
```

### Writing Data

#### `write_byte(fileId, value)`
Write single byte (0-255).

```bulang
file.write_byte(f, 255);
```

#### `write_int(fileId, value)`
Write 32-bit integer.

```bulang
file.write_int(f, 1234567);
```

#### `write_float(fileId, value)`
Write 32-bit float.

```bulang
file.write_float(f, 3.14159);
```

#### `write_double(fileId, value)`
Write 64-bit double.

```bulang
file.write_double(f, 3.141592653589793);
```

#### `write_bool(fileId, value)`
Write boolean as byte.

```bulang
file.write_bool(f, true);
```

#### `write_string(fileId, value)`
Write string (length-prefixed).

```bulang
file.write_string(f, "Hello, World!");
```

### Reading Data

#### `read_byte(fileId)`
Read single byte.

```bulang
value = file.read_byte(f);  # 0-255
```

#### `read_int(fileId)`
Read 32-bit integer.

```bulang
value = file.read_int(f);
```

#### `read_float(fileId)`
Read 32-bit float.

```bulang
value = file.read_float(f);
```

#### `read_double(fileId)`
Read 64-bit double.

```bulang
value = file.read_double(f);
```

#### `read_bool(fileId)`
Read boolean.

```bulang
value = file.read_bool(f);
```

#### `read_string(fileId)`
Read string (length-prefixed).

```bulang
text = file.read_string(f);
```

### Cursor Control

#### `seek(fileId, position)`
Move cursor to position.

```bulang
file.seek(f, 0);     # Start of file
file.seek(f, 100);   # Position 100
```

#### `tell(fileId)`
Get current cursor position.

```bulang
pos = file.tell(f);
```

#### `size(fileId)`
Get file size in bytes.

```bulang
bytes = file.size(f);
```

### Example: Save Game

```bulang
import file;

# Save
f = file.open("savegame.dat", "w");
file.write_int(f, playerHealth);
file.write_int(f, playerScore);
file.write_string(f, playerName);
file.close(f);

# Load
f = file.open("savegame.dat", "r");
health = file.read_int(f);
score = file.read_int(f);
name = file.read_string(f);
file.close(f);
```

---

## 🗂️ FileSystem (fs) Module

High-level text file and directory operations.

```bulang
import fs;
```

### File Operations

#### `read(path)`
Read entire file as string.

```bulang
content = fs.read("config.txt");
if content != nil {
    print(content);
}
```

#### `write(path, data)`
Write string to file (overwrites).

```bulang
fs.write("output.txt", "Hello, World!");
```

#### `append(path, data)`
Append string to file.

```bulang
fs.append("log.txt", "New entry\n");
```

#### `remove(path)`
Delete file.

```bulang
fs.remove("temp.txt");
```

### Directory Operations

#### `mkdir(path)`
Create directory.

```bulang
fs.mkdir("data");
fs.mkdir("saves/profile1");
```

#### `rmdir(path)`
Remove empty directory.

```bulang
fs.rmdir("old_data");
```

#### `list(path)`
List directory contents.

```bulang
files = fs.list(".");
for file in files {
    print(file);
}
```

### File Information

#### `stat(path)`
Get file/directory information.

```bulang
info = fs.stat("data.txt");
if info != nil {
    print("Size: {} bytes", info["size"]);
    print("Is file: {}", info["isfile"]);
    print("Is dir: {}", info["isdir"]);
    # Unix only:
    # print("Mode: {}", info["mode"]);
    # print("Modified: {}", info["mtime"]);
}
```

---

## 🛤️ Path Module

Cross-platform path manipulation.

```bulang
import path;
```

### Path Operations

#### `join(...paths)`
Join path components.

```bulang
fullPath = path.join("data", "saves", "player.dat");
# Windows: "data\saves\player.dat"
# Unix: "data/saves/player.dat"
```

#### `normalize(path)`
Normalize path (resolve . and ..).

```bulang
clean = path.normalize("data/../config/./settings.txt");
# "config/settings.txt"
```

#### `dirname(path)`
Get directory portion of path.

```bulang
dir = path.dirname("/home/user/file.txt");
# "/home/user"
```

#### `basename(path)`
Get filename portion of path.

```bulang
name = path.basename("/home/user/file.txt");
# "file.txt"
```

#### `extname(path)`
Get file extension.

```bulang
ext = path.extname("document.pdf");
# ".pdf"

ext = path.extname("archive.tar.gz");
# ".gz"
```

### Path Checks

#### `exists(path)`
Check if path exists.

```bulang
if path.exists("config.txt") {
    print("Config found!");
}
```

#### `isdir(path)`
Check if path is directory.

```bulang
if path.isdir("saves") {
    print("Saves directory exists");
}
```

#### `isfile(path)`
Check if path is regular file.

```bulang
if path.isfile("data.bin") {
    print("Data file exists");
}
```

---

## 💻 OS Module

Operating system interaction and process management.

```bulang
import os;
```

### Platform Information

#### `os.platform`
Current platform string.

```bulang
print(os.platform);
# "windows", "linux", "macos", "unix", "android", or "unknown"
```

### Environment Variables

#### `getenv(name)`
Get environment variable.

```bulang
home = os.getenv("HOME");
path = os.getenv("PATH");
```

#### `setenv(name, value)`
Set environment variable.

```bulang
os.setenv("MY_VAR", "value");
```

### Directory Navigation

#### `getcwd()`
Get current working directory.

```bulang
cwd = os.getcwd();
print("Working directory: {}", cwd);
```

#### `chdir(path)`
Change working directory.

```bulang
os.chdir("/home/user/project");
```

### Process Management

#### `execute(command)`
Execute command synchronously (blocks until complete).

```bulang
result = os.execute("ls -la");
print("Exit code: {}", result);
```

#### `spawn(command, [args...])`
Spawn process asynchronously. Returns process ID.

```bulang
pid = os.spawn("python", "script.py", "--verbose");
print("Started process {}", pid);
```

#### `spawn_shell(command)`
Execute command via system shell.

```bulang
pid = os.spawn_shell("echo 'Hello' | grep H");
```

#### `spawn_capture(command)`
Execute and capture output.

```bulang
result = os.spawn_capture("git status");
print("Output: {}", result["output"]);
print("Exit code: {}", result["code"]);
```

#### `wait(pid)`
Wait for process to complete. Returns exit code.

```bulang
pid = os.spawn("converter", "input.txt");
exitCode = os.wait(pid);
if exitCode == 0 {
    print("Success!");
}
```

#### `kill(pid)`
Terminate process.

```bulang
os.kill(pid);
```

#### `quit(exitCode)`
Exit program with code.

```bulang
os.quit(0);  # Success
os.quit(1);  # Error
```

---

## 🌐 Socket Module

Network communication (TCP, UDP, HTTP).

```bulang
import socket;
```

### Initialization

#### `init()`
Initialize socket system (Windows only, automatic on Unix).

```bulang
socket.init();
```

#### `quit()`
Cleanup socket system.

```bulang
socket.quit();
```

### HTTP Requests (High-Level)

#### `http_get(url, [options])`
Perform HTTP GET request.

```bulang
# Simple GET
response = socket.http_get("http://api.example.com/data");

# With options
response = socket.http_get("http://api.example.com/users", {
    params: {
        page: 1,
        limit: 10
    },
    headers: {
        "Authorization": "Bearer token123",
        "Accept": "application/json"
    },
    timeout: 30,
    user_agent: "MyApp/1.0"
});

# Access response
if response["success"] {
    print("Status: {}", response["status_code"]);
    print("Body: {}", response["body"]);
    print("Headers: {}", response["headers"]["Content-Type"]);
}
```

#### `http_post(url, [options])`
Perform HTTP POST request.

```bulang
# Form data
response = socket.http_post("http://api.example.com/login", {
    data: {
        username: "admin",
        password: "secret"
    }
});

# JSON data (auto-serialized)
response = socket.http_post("http://api.example.com/users", {
    json: {
        name: "John Doe",
        email: "john@example.com",
        age: 30
    },
    headers: {
        "Authorization": "Bearer token"
    }
});

# Raw string
response = socket.http_post("http://api.example.com/webhook", {
    data: '{"event": "user.created"}',
    headers: {
        "Content-Type": "application/json"
    }
});
```

Response structure:
```bulang
{
    status_code: 200,
    status_text: "OK",
    success: true,
    body: "...",
    url: "...",
    headers: { ... }
}
```

#### `download_file(url, filepath)`
Download file to disk.

```bulang
success = socket.download_file(
    "http://example.com/file.zip",
    "./downloads/file.zip"
);
```

### TCP Sockets

#### `tcp_listen(port, [backlog])`
Create TCP server. Returns socket ID.

```bulang
server = socket.tcp_listen(8080, 10);
socket.set_blocking(server, false);

while true {
    client = socket.tcp_accept(server);
    if client != nil {
        print("Client connected!");
    }
}
```

#### `tcp_accept(serverSocketId)`
Accept incoming connection. Returns client socket ID or nil.

```bulang
client = socket.tcp_accept(server);
```

#### `tcp_connect(host, port)`
Connect to TCP server. Returns socket ID.

```bulang
sock = socket.tcp_connect("example.com", 80);
if sock != nil {
    socket.send(sock, "GET / HTTP/1.1\r\n\r\n");
    response = socket.receive(sock);
    socket.close(sock);
}
```

### UDP Sockets

#### `udp_create(port)`
Create UDP socket. Returns socket ID.

```bulang
udp = socket.udp_create(5000);
```

#### `sendto(socketId, data, host, port)`
Send UDP packet.

```bulang
sent = socket.sendto(udp, "Hello", "192.168.1.100", 5001);
```

#### `recvfrom(socketId, [maxSize])`
Receive UDP packet.

```bulang
packet = socket.recvfrom(udp, 1024);
if packet != nil {
    print("From: {}:{}", packet["host"], packet["port"]);
    print("Data: {}", packet["data"]);
}
```

### Socket I/O

#### `send(socketId, data)`
Send data on TCP socket. Returns bytes sent.

```bulang
sent = socket.send(sock, "Hello, Server!");
```

#### `receive(socketId, [maxSize])`
Receive data from TCP socket. Returns string or nil.

```bulang
data = socket.receive(sock, 4096);
if data != nil {
    print("Received: {}", data);
}
```

### Socket Configuration

#### `set_blocking(socketId, blocking)`
Set blocking/non-blocking mode.

```bulang
socket.set_blocking(sock, false);  # Non-blocking
```

#### `set_nodelay(socketId, nodelay)`
Set TCP_NODELAY (disable Nagle's algorithm).

```bulang
socket.set_nodelay(sock, true);  # Low latency
```

### Socket Information

#### `info(socketId)`
Get socket information.

```bulang
info = socket.info(sock);
print("Type: {}", info["type"]);        # "tcp_client", "tcp_server", "udp"
print("Port: {}", info["port"]);
print("Connected: {}", info["connected"]);
print("Blocking: {}", info["blocking"]);
```

#### `is_connected(socketId)`
Check if socket is connected.

```bulang
if socket.is_connected(sock) {
    socket.send(sock, "ping");
}
```

#### `close(socketId)`
Close socket.

```bulang
socket.close(sock);
```

### Network Utilities

#### `ping(host, [port], [timeout])`
Check if host is reachable.

```bulang
if socket.ping("google.com", 80, 2) {
    print("Host is up!");
}
```

#### `resolve(hostname)`
Resolve hostname to IP address.

```bulang
ip = socket.resolve("google.com");
print("IP: {}", ip);  # e.g., "142.250.184.46"
```

#### `get_local_ip()`
Get local machine IP address.

```bulang
myIp = socket.get_local_ip();
print("My IP: {}", myIp);
```

### Complete Examples

**HTTP API Client:**
```bulang
import socket;

socket.init();

# GET request
users = socket.http_get("https://api.example.com/users", {
    params: { page: 1 },
    headers: { "Authorization": "Bearer token" }
});

if users["success"] {
    print("Users: {}", users["body"]);
}

# POST request
response = socket.http_post("https://api.example.com/users", {
    json: {
        name: "Alice",
        email: "alice@example.com"
    }
});

socket.quit();
```

**TCP Echo Server:**
```bulang
import socket;

socket.init();

server = socket.tcp_listen(8080);
socket.set_blocking(server, false);

print("Server listening on port 8080");

clients = [];

while true {
    # Accept new clients
    client = socket.tcp_accept(server);
    if client != nil {
        socket.set_blocking(client, false);
        clients.push(client);
        print("Client connected");
    }
    
    # Handle existing clients
    for i in 0..len(clients) {
        data = socket.receive(clients[i]);
        if data != nil {
            print("Received: {}", data);
            socket.send(clients[i], data);  # Echo back
        }
    }
    
    time.sleep_ms(10);
}
```

**UDP Chat:**
```bulang
import socket;

socket.init();

udp = socket.udp_create(5000);
socket.set_blocking(udp, false);

while true {
    # Receive messages
    packet = socket.recvfrom(udp);
    if packet != nil {
        print("[{}]: {}", packet["host"], packet["data"]);
    }
    
    # Send message
    socket.sendto(udp, "Hello!", "192.168.1.100", 5000);
    
    time.sleep(1);
}
```

---

## 📝 Notes

- All modules must be imported with `import` or `using` before use
- Functions returning `nil` indicate failure or no data available
- Non-blocking sockets require manual polling
- HTTP functions only support HTTP (not HTTPS yet)
- File paths are platform-specific (use `path` module for portability)

---

## 🔗 See Also

- [Language Reference](docs/language.md)
- [Examples](examples/)
- [Playground](https://bulang.netlify.app/playground.html)
