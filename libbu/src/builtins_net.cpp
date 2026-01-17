
#include "interpreter.hpp"
#include "platform.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

// ============================================
// SOCKET MODULE - COMPLETO
// ============================================

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#define SHUT_RDWR SD_BOTH
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
typedef int SOCKET;
#endif

#include <cstring>
#include <vector>
#include <string>

enum class SocketType
{
    TCP_SERVER,
    TCP_CLIENT,
    UDP
};

struct SocketHandle
{
    SOCKET socket;
    SocketType type;
    bool isBlocking;
    bool isConnected;
    uint16_t port;
    std::string host;
};

static std::vector<SocketHandle *> openSockets;
static int nextSocketId = 1;
static bool wsaInitialized = false;

static void SocketModuleCleanup()
{
    for (auto handle : openSockets)
    {
        if (handle && handle->socket != INVALID_SOCKET)
        {
            shutdown(handle->socket, SHUT_RDWR);
            closesocket(handle->socket);
            delete handle;
        }
    }
    openSockets.clear();

#ifdef _WIN32
    if (wsaInitialized)
    {
        WSACleanup();
        wsaInitialized = false;
    }
#endif
}

Value native_socket_init(Interpreter *vm, int argCount, Value *args)
{
#ifdef _WIN32
    if (!wsaInitialized)
    {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
        {
            vm->runtimeError("WSAStartup failed: %d", result);
            return vm->makeBool(false);
        }
        wsaInitialized = true;
    }
#endif
    return vm->makeBool(true);
}

Value native_socket_quit(Interpreter *vm, int argCount, Value *args)
{
    SocketModuleCleanup();
    return vm->makeNil();
}

Value native_socket_tcp_listen(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
    {
        vm->runtimeError("tcp_listen expects port number");
        return vm->makeNil();
    }

    int port = args[0].asInt();
    int backlog = 10;

    if (argCount >= 2 && args[1].isInt())
        backlog = args[1].asInt();

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        vm->runtimeError("Failed to create socket");
        return vm->makeNil();
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        closesocket(sock);
        vm->runtimeError("Failed to bind to port %d", port);
        return vm->makeNil();
    }

    if (listen(sock, backlog) == SOCKET_ERROR)
    {
        closesocket(sock);
        vm->runtimeError("Failed to listen on port %d", port);
        return vm->makeNil();
    }

    SocketHandle *handle = new SocketHandle();
    handle->socket = sock;
    handle->type = SocketType::TCP_SERVER;
    handle->isBlocking = true;
    handle->isConnected = true;
    handle->port = port;

    openSockets.push_back(handle);
    return vm->makeInt(nextSocketId++);
}

Value native_socket_tcp_accept(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();

    int id = args[0].asInt();
    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeNil();

    SocketHandle *serverHandle = openSockets[id - 1];

    if (serverHandle->type != SocketType::TCP_SERVER)
    {
        vm->runtimeError("Socket is not a TCP server");
        return vm->makeNil();
    }

    sockaddr_in clientAddr = {0};
    socklen_t addrLen = sizeof(clientAddr);

    SOCKET clientSock = accept(serverHandle->socket, (sockaddr *)&clientAddr, &addrLen);

    if (clientSock == INVALID_SOCKET)
    {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
            return vm->makeNil();
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return vm->makeNil();
#endif
        return vm->makeNil();
    }

    SocketHandle *clientHandle = new SocketHandle();
    clientHandle->socket = clientSock;
    clientHandle->type = SocketType::TCP_CLIENT;
    clientHandle->isBlocking = true;
    clientHandle->isConnected = true;
    clientHandle->port = ntohs(clientAddr.sin_port);
    clientHandle->host = inet_ntoa(clientAddr.sin_addr);

    openSockets.push_back(clientHandle);
    return vm->makeInt(nextSocketId++);
}

Value native_socket_tcp_connect(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isString() || !args[1].isInt())
    {
        vm->runtimeError("tcp_connect expects (host, port)");
        return vm->makeNil();
    }

    const char *host = args[0].asStringChars();
    int port = args[1].asInt();

    struct hostent *he = gethostbyname(host);
    if (!he)
    {
        vm->runtimeError("Failed to resolve hostname '%s'", host);
        return vm->makeNil();
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        vm->runtimeError("Failed to create socket");
        return vm->makeNil();
    }

    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    if (connect(sock, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK)
        {
            closesocket(sock);
            vm->runtimeError("Failed to connect to %s:%d", host, port);
            return vm->makeNil();
        }
#else
        if (errno != EINPROGRESS)
        {
            closesocket(sock);
            vm->runtimeError("Failed to connect to %s:%d", host, port);
            return vm->makeNil();
        }
#endif
    }

    SocketHandle *handle = new SocketHandle();
    handle->socket = sock;
    handle->type = SocketType::TCP_CLIENT;
    handle->isBlocking = true;
    handle->isConnected = true;
    handle->port = port;
    handle->host = host;

    openSockets.push_back(handle);
    return vm->makeInt(nextSocketId++);
}

Value native_socket_udp_create(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
    {
        vm->runtimeError("udp_create expects port");
        return vm->makeNil();
    }

    int port = args[0].asInt();

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        vm->runtimeError("Failed to create UDP socket");
        return vm->makeNil();
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        closesocket(sock);
        vm->runtimeError("Failed to bind UDP socket to port %d", port);
        return vm->makeNil();
    }

    SocketHandle *handle = new SocketHandle();
    handle->socket = sock;
    handle->type = SocketType::UDP;
    handle->isBlocking = true;
    handle->isConnected = false;
    handle->port = port;

    openSockets.push_back(handle);
    return vm->makeInt(nextSocketId++);
}

Value native_socket_set_blocking(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isBool())
        return vm->makeBool(false);

    int id = args[0].asInt();
    bool blocking = args[1].asBool();

    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeBool(false);

    SocketHandle *handle = openSockets[id - 1];

#ifdef _WIN32
    u_long mode = blocking ? 0 : 1;
    if (ioctlsocket(handle->socket, FIONBIO, &mode) != 0)
        return vm->makeBool(false);
#else
    int flags = fcntl(handle->socket, F_GETFL, 0);
    if (flags == -1)
        return vm->makeBool(false);

    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    if (fcntl(handle->socket, F_SETFL, flags) != 0)
        return vm->makeBool(false);
#endif

    handle->isBlocking = blocking;
    return vm->makeBool(true);
}

Value native_socket_set_nodelay(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isBool())
        return vm->makeBool(false);

    int id = args[0].asInt();
    bool nodelay = args[1].asBool();

    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeBool(false);

    SocketHandle *handle = openSockets[id - 1];

    if (handle->type == SocketType::UDP)
        return vm->makeBool(false);

    int flag = nodelay ? 1 : 0;
    if (setsockopt(handle->socket, IPPROTO_TCP, TCP_NODELAY,
                   (char *)&flag, sizeof(flag)) != 0)
        return vm->makeBool(false);

    return vm->makeBool(true);
}

Value native_socket_send(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 2 || !args[0].isInt() || !args[1].isString())
        return vm->makeInt(-1);

    int id = args[0].asInt();
    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeInt(-1);

    SocketHandle *handle = openSockets[id - 1];

    if (handle->type == SocketType::UDP)
    {
        vm->runtimeError("Use sendto() for UDP sockets");
        return vm->makeInt(-1);
    }

    if (!handle->isConnected)
    {
        vm->runtimeError("Socket not connected");
        return vm->makeInt(-1);
    }

    const char *data = args[1].asStringChars();
    int len = args[1].asString()->length();

    int sent = send(handle->socket, data, len, 0);

    if (sent == SOCKET_ERROR)
    {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
            return vm->makeInt(0);
        handle->isConnected = false;
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return vm->makeInt(0);
        handle->isConnected = false;
#endif
        return vm->makeInt(-1);
    }

    return vm->makeInt(sent);
}

Value native_socket_receive(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();

    int id = args[0].asInt();
    int maxSize = 4096;

    if (argCount >= 2 && args[1].isInt())
        maxSize = args[1].asInt();

    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeNil();

    SocketHandle *handle = openSockets[id - 1];

    if (handle->type == SocketType::UDP)
    {
        vm->runtimeError("Use recvfrom() for UDP sockets");
        return vm->makeNil();
    }

    std::vector<char> buffer(maxSize);
    int received = recv(handle->socket, buffer.data(), maxSize, 0);

    if (received == SOCKET_ERROR)
    {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err == WSAEWOULDBLOCK)
            return vm->makeNil();
        handle->isConnected = false;
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return vm->makeNil();
        handle->isConnected = false;
#endif
        return vm->makeNil();
    }

    if (received == 0)
    {
        handle->isConnected = false;
        return vm->makeNil();
    }

    return vm->makeString(std::string(buffer.data(), received).c_str());
}

Value native_socket_sendto(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 4 || !args[0].isInt() || !args[1].isString() ||
        !args[2].isString() || !args[3].isInt())
    {
        vm->runtimeError("sendto expects (socketId, data, host, port)");
        return vm->makeInt(-1);
    }

    int id = args[0].asInt();
    const char *data = args[1].asStringChars();
    const char *host = args[2].asStringChars();
    int port = args[3].asInt();

    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeInt(-1);

    SocketHandle *handle = openSockets[id - 1];

    if (handle->type != SocketType::UDP)
    {
        vm->runtimeError("sendto() is for UDP sockets only");
        return vm->makeInt(-1);
    }

    struct hostent *he = gethostbyname(host);
    if (!he)
        return vm->makeInt(-1);

    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);

    int len = args[1].asString()->length();
    int sent = sendto(handle->socket, data, len, 0, (sockaddr *)&addr, sizeof(addr));

    if (sent == SOCKET_ERROR)
        return vm->makeInt(-1);

    return vm->makeInt(sent);
}

Value native_socket_recvfrom(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();

    int id = args[0].asInt();
    int maxSize = 4096;

    if (argCount >= 2 && args[1].isInt())
        maxSize = args[1].asInt();

    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeNil();

    SocketHandle *handle = openSockets[id - 1];

    if (handle->type != SocketType::UDP)
    {
        vm->runtimeError("recvfrom() is for UDP sockets only");
        return vm->makeNil();
    }

    std::vector<char> buffer(maxSize);
    sockaddr_in fromAddr = {0};
    socklen_t fromLen = sizeof(fromAddr);

    int received = recvfrom(handle->socket, buffer.data(), maxSize, 0,
                            (sockaddr *)&fromAddr, &fromLen);

    if (received == SOCKET_ERROR)
    {
#ifdef _WIN32
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            return vm->makeNil();
#else
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return vm->makeNil();
#endif
        return vm->makeNil();
    }

    Value result = vm->makeMap();
    MapInstance *map = result.asMap();

    map->table.set(vm->makeString("data").asString(),
                   vm->makeString(std::string(buffer.data(), received).c_str()));
    map->table.set(vm->makeString("host").asString(),
                   vm->makeString(inet_ntoa(fromAddr.sin_addr)));
    map->table.set(vm->makeString("port").asString(),
                   vm->makeInt(ntohs(fromAddr.sin_port)));

    return result;
}

Value native_socket_info(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeNil();

    int id = args[0].asInt();
    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeNil();

    SocketHandle *handle = openSockets[id - 1];

    Value result = vm->makeMap();
    MapInstance *map = result.asMap();

    const char *typeStr = "unknown";
    if (handle->type == SocketType::TCP_SERVER)
        typeStr = "tcp_server";
    else if (handle->type == SocketType::TCP_CLIENT)
        typeStr = "tcp_client";
    else if (handle->type == SocketType::UDP)
        typeStr = "udp";

    map->table.set(vm->makeString("type").asString(), vm->makeString(typeStr));
    map->table.set(vm->makeString("port").asString(), vm->makeInt(handle->port));
    map->table.set(vm->makeString("blocking").asString(), vm->makeBool(handle->isBlocking));
    map->table.set(vm->makeString("connected").asString(), vm->makeBool(handle->isConnected));

    if (!handle->host.empty())
        map->table.set(vm->makeString("host").asString() , vm->makeString(handle->host.c_str()));

    return result;
}

Value native_socket_close(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeBool(false);

    int id = args[0].asInt();
    if (id <= 0 || id > openSockets.size() || !openSockets[id - 1])
        return vm->makeBool(false);

    SocketHandle *handle = openSockets[id - 1];

    if (handle->type != SocketType::UDP)
        shutdown(handle->socket, SHUT_RDWR);

    closesocket(handle->socket);
    delete handle;
    openSockets[id - 1] = nullptr;

    return vm->makeBool(true);
}

 Value native_socket_is_connected(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || !args[0].isInt())
        return vm->makeBool(false);
    
    int id = args[0].asInt();
    if (id <= 0 || id > openSockets.size() || !openSockets[id-1])
        return vm->makeBool(false);
    
    SocketHandle *handle = openSockets[id-1];
    return vm->makeBool(handle->isConnected);
}

// No registerSocket():
             
void Interpreter::registerSocket()
{
    static bool initialized = false;
    if (!initialized)
    {
        atexit(SocketModuleCleanup);
        initialized = true;
    }

    addModule("socket")
        .addFunction("init", native_socket_init, 0)
        .addFunction("quit", native_socket_quit, 0)

        .addFunction("tcp_listen", native_socket_tcp_listen, -1)
        .addFunction("tcp_accept", native_socket_tcp_accept, 1)
        .addFunction("tcp_connect", native_socket_tcp_connect, 2)

        .addFunction("udp_create", native_socket_udp_create, 1)

        .addFunction("send", native_socket_send, 2)
        .addFunction("receive", native_socket_receive, -1)
        .addFunction("sendto", native_socket_sendto, 4)
        .addFunction("recvfrom", native_socket_recvfrom, -1)
 
        .addFunction("is_connected", native_socket_is_connected, 1)

        .addFunction("set_blocking", native_socket_set_blocking, 2)
        .addFunction("set_nodelay", native_socket_set_nodelay, 2)

        .addFunction("info", native_socket_info, 1)
        .addFunction("close", native_socket_close, 1);
}