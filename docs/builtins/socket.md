# Socket Module

```bulang
import socket;
```

Network operations: TCP, UDP, HTTP.

## Initialization

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `init` | none | `bool` | Initialize networking (Windows) |
| `quit` | none | `nil` | Cleanup networking |

## TCP Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `tcp_listen` | `port: int` | `int` | Create listening socket |
| `tcp_accept` | `socket: int` | `int` | Accept connection |
| `tcp_connect` | `host: string`, `port: int` | `int` | Connect to server |

## UDP Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `udp_create` | `port: int` | `int` | Create UDP socket bound to port |
| `sendto` | `socket`, `data`, `host`, `port` | `int` | Send UDP packet |
| `recvfrom` | `socket: int`, `size?: int` | `map\|nil` | Receive UDP packet |

## Send/Receive

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `send` | `socket: int`, `data: string` | `int` | Send data |
| `receive` | `socket: int`, `size: int` | `string` | Receive data |

## HTTP Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `http_get` | `url: string`, `options?: map` | `map` | HTTP GET request |
| `http_post` | `url: string`, `options?: map` | `map` | HTTP POST request |
| `download_file` | `url: string`, `path: string` | `bool` | Download file to disk |

### HTTP Options Map

```bulang
{
    headers: map,      // Custom headers {"Key": "Value"}
    params: map,       // Query parameters (for GET)
    data: string|map,  // POST body (form-encoded if map)
    json: any,         // POST body as JSON (auto-serialized)
    timeout: int,      // Timeout in seconds (default: 30)
    user_agent: string // Custom User-Agent
}
```

### HTTP Response Map

```bulang
{
    status_code: int,   // HTTP status (200, 404, etc.)
    status_text: string,// Status message ("OK", "Not Found")
    body: string,       // Response body
    success: bool,      // true if 2xx status
    headers: map,       // Response headers
    url: string         // Final URL
}
```

## Utility Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `ping` | `host`, `port?`, `timeout?` | `bool` | Check host reachable |
| `resolve` | `hostname: string` | `string` | DNS lookup |
| `get_local_ip` | none | `string` | Get local IP address |
| `info` | `socket: int` | `map` | Get socket info |
| `close` | `socket: int` | `bool` | Close socket |

## Socket Options

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `set_blocking` | `socket: int`, `blocking: bool` | `bool` | Set blocking mode |
| `set_nodelay` | `socket: int`, `nodelay: bool` | `bool` | Disable Nagle's algorithm |
| `is_connected` | `socket: int` | `bool` | Check connection status |

## Examples

### TCP Server

```bulang
import socket;

socket.init();

var server = socket.tcp_listen(8080);
print("Listening on port 8080");

var client = socket.tcp_accept(server);
print("Client connected");

var data = socket.receive(client, 1024);
print("Received: " + data);

socket.send(client, "Hello from server!");

socket.close(client);
socket.close(server);
socket.quit();
```

### TCP Client

```bulang
import socket;

socket.init();

var sock = socket.tcp_connect("127.0.0.1", 8080);
socket.send(sock, "Hello from client!");

var response = socket.receive(sock, 1024);
print("Response: " + response);

socket.close(sock);
socket.quit();
```

### HTTP Requests

```bulang
import socket;

// Simple GET
var resp = socket.http_get("http://api.example.com/users");
if (resp.success) {
    print("Body: " + resp.body);
}

// GET with headers and params
resp = socket.http_get("http://api.example.com/search", {
    params: {q: "test", page: 1},
    headers: {"Authorization": "Bearer token123"},
    timeout: 10
});

// POST with form data
resp = socket.http_post("http://api.example.com/login", {
    data: {username: "admin", password: "secret"}
});

// POST with JSON (auto-serialized)
resp = socket.http_post("http://api.example.com/users", {
    json: {name: "John", email: "john@example.com"},
    headers: {"Authorization": "Bearer token123"}
});

// Access response
print("Status: " + str(resp.status_code));
print("Body: " + resp.body);
print("Content-Type: " + resp.headers["Content-Type"]);

// Download file
socket.download_file("http://example.com/file.zip", "file.zip");
```

### UDP Communication

```bulang
import socket;

socket.init();

// Sender
var udp = socket.udp_create(0);  // bind to any port
socket.sendto(udp, "Hello UDP!", "127.0.0.1", 9000);
socket.close(udp);

// Receiver
var receiver = socket.udp_create(9000);
var packet = socket.recvfrom(receiver, 1024);
if (packet != nil) {
    print("From: " + packet.host + ":" + str(packet.port));
    print("Data: " + packet.data);
}
socket.close(receiver);

socket.quit();
```

### DNS and Ping

```bulang
import socket;

// DNS lookup
var ip = socket.resolve("google.com");
print("IP: " + ip);

// Check reachable (host, port, timeout)
if (socket.ping("google.com", 80, 5)) {
    print("Google is reachable");
}

// Local IP
print("My IP: " + socket.get_local_ip());
```

## Notes

- Call `init()` on Windows before using sockets
- Always `close()` sockets when done
- **HTTPS is not supported** - use HTTP only
- `receive()` blocks until data or disconnect (returns nil on non-blocking timeout)
- Use `set_blocking(sock, false)` for non-blocking I/O
- HTTP functions return maps with status_code, body, headers, etc.
- `ping()` tests TCP connectivity, not ICMP ping
