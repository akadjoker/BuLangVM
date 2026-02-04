# BuLang Builtins

Este documento resume os builtins expostos ao script e inclui:
- exemplos rapidos de uso
- tabela de aridade/retorno
- nota sobre funcoes matematicas que viram opcode

## Nota Importante: Funcoes viram Opcode
Algumas funcoes matematicas e utilitarias sao compiladas direto para opcode
(nao passam pelo modulo `math`). Exemplo: `sin(x)` vira `OP_SIN`.

Funcoes opcode-backend:
- `len(x)`
- `free(x)`
- `clock()`
- `sin(x)`, `cos(x)`, `tan(x)`
- `asin(x)`, `acos(x)`, `atan(x)`
- `atan2(y, x)`
- `sqrt(x)`
- `log(x)`
- `pow(a, b)`
- `abs(x)`
- `floor(x)`, `ceil(x)`
- `deg(x)`, `rad(x)`
- `exp(x)`

---

## Exemplos Rapidos

### Globais
```bu
print(format("pos {} {}", 10, 20));
write("loading {}%", 42);
var name = input("nome: ");
print_stack();
str(123); int("42"); real("3.14");
```

### math
```bu
print(math.lerp(0, 10, 0.5));
print(math.clamp(15, 0, 10));
print(math.rand(0, 1));
```

### time
```bu
var t = time.now();
time.sleep(1);
print(time.diff(time.now(), t));
```

### path
```bu
print(path.join("a", "b", "c.txt"));
print(path.extname("file.png"));
```

### fs
```bu
fs.write("a.txt", "ola");
print(fs.read("a.txt"));
print(fs.stat("a.txt")); // map com size/isdir/isfile/etc
```

### file
```bu
var f = file.open("data.bin", "wb");
file.write_int(f, 123);
file.close(f);
```

### os
```bu
print(os.platform);
print(os.getcwd());
os.execute("echo hi");
```

### socket
```bu
socket.init();
var r = socket.http_get("http://example.com");
print(r.status_code, r.body);
socket.quit();
```

---

## Tabela (Aridade e Retorno)

### Globais
| Funcao | Aridade | Retorno |
|---|---|---|
| `format(...)` | -1 | `string` |
| `write(...)` | -1 | `nil` |
| `input([prompt])` | -1 | `string` ou `nil` |
| `print_stack(...)` | -1 | `nil` |
| `ticks(dt)` | 1 | `nil` |
| `_gc()` | 0 | `nil` |
| `str(x)` | 1 | `string` |
| `int(x)` | 1 | `int` |
| `real(x)` | 1 | `double` |

### math
| Funcao | Aridade | Retorno |
|---|---|---|
| `lerp(a,b,t)` | 3 | `double` |
| `map(v, inMin, inMax, outMin, outMax)` | 5 | `double` |
| `sign(x)` | 1 | `int` |
| `hypot(x, y)` | 2 | `double` |
| `log10(x)` | 1 | `double` |
| `log2(x)` | 1 | `double` |
| `sinh(x)` | 1 | `double` |
| `cosh(x)` | 1 | `double` |
| `tanh(x)` | 1 | `double` |
| `smoothstep(...)` | -1 | `double` |
| `smootherstep(...)` | -1 | `double` |
| `hermite(a,b,c,d,t)` | 5 | `double` |
| `repeat(t, length)` | 2 | `double` |
| `ping_pong(t, length)` | 2 | `double` |
| `clamp(x, min, max)` | 3 | `double` |
| `min(a,b)` | 2 | `double` |
| `max(a,b)` | 2 | `double` |
| `seed(n)` | 1 | `nil` |
| `rand([min],[max])` | -1 | `double` |
| `irand([min],[max])` | -1 | `int` |

### time
| Funcao | Aridade | Retorno |
|---|---|---|
| `time.now()` | 0 | `int` |
| `time.now_ms()` | 0 | `int` |
| `time.current()` | 0 | `double` |
| `time.sleep(sec)` | 1 | `nil` |
| `time.sleep_ms(ms)` | 1 | `nil` |
| `time.date(...)` | -1 | `map` |
| `time.ftime(...)` | -1 | `string` |
| `time.parse(str, fmt)` | 2 | `int` |
| `time.diff(t1, t2)` | 2 | `int` |

### path
| Funcao | Aridade | Retorno |
|---|---|---|
| `path.join(...)` | -1 | `string` |
| `path.normalize(path)` | 1 | `string` |
| `path.dirname(path)` | 1 | `string` |
| `path.basename(path)` | 1 | `string` |
| `path.extname(path)` | 1 | `string` |
| `path.exists(path)` | 1 | `bool` |
| `path.isdir(path)` | 1 | `bool` |
| `path.isfile(path)` | 1 | `bool` |

### fs
| Funcao | Aridade | Retorno |
|---|---|---|
| `fs.read(path)` | 1 | `string` ou `nil` |
| `fs.write(path, data)` | 2 | `bool` |
| `fs.append(path, data)` | 2 | `bool` |
| `fs.remove(path)` | 1 | `bool` |
| `fs.mkdir(path)` | 1 | `bool` |
| `fs.rmdir(path)` | 1 | `bool` |
| `fs.list(path)` | 1 | `array` |
| `fs.stat(path)` | 1 | `map` ou `nil` |

### file
| Funcao | Aridade | Retorno |
|---|---|---|
| `file.exists(path)` | 1 | `bool` |
| `file.open(path, mode, ...)` | -1 | `int` (handle) |
| `file.save(path)` | 1 | `bool` |
| `file.close(handle)` | 1 | `bool` |
| `file.write_byte(handle, v)` | 2 | `bool` |
| `file.write_int(handle, v)` | 2 | `bool` |
| `file.write_float(handle, v)` | 2 | `bool` |
| `file.write_double(handle, v)` | 2 | `bool` |
| `file.write_bool(handle, v)` | 2 | `bool` |
| `file.write_string(handle, v)` | 2 | `bool` |
| `file.read_byte(handle)` | 1 | `int` |
| `file.read_int(handle)` | 1 | `int` |
| `file.read_float(handle)` | 1 | `double` |
| `file.read_double(handle)` | 1 | `double` |
| `file.read_bool(handle)` | 1 | `bool` |
| `file.read_string(handle)` | 1 | `string` |
| `file.seek(handle, pos)` | 2 | `bool` |
| `file.tell(handle)` | 1 | `int` |
| `file.size(handle)` | 1 | `int` |

### os
| Funcao | Aridade | Retorno |
|---|---|---|
| `os.spawn(...)` | -1 | `int` (pid) |
| `os.spawn_shell(cmd)` | 1 | `int` (pid) |
| `os.spawn_capture(cmd)` | 1 | `map` |
| `os.wait(pid)` | 1 | `int` |
| `os.kill(pid)` | 1 | `bool` |
| `os.execute(cmd)` | 1 | `int` |
| `os.getenv(name)` | 1 | `string` ou `nil` |
| `os.setenv(name, value)` | 2 | `bool` |
| `os.getcwd()` | 0 | `string` |
| `os.chdir(path)` | 1 | `bool` |
| `os.quit(code)` | 1 | `nil` |

### socket
| Funcao | Aridade | Retorno |
|---|---|---|
| `socket.init()` | 0 | `bool` |
| `socket.quit()` | 0 | `nil` |
| `socket.tcp_listen(...)` | -1 | `int` (socket id) |
| `socket.tcp_accept(id)` | 1 | `int` (client id) |
| `socket.tcp_connect(host, port)` | 2 | `int` (socket id) |
| `socket.udp_create(port)` | 1 | `int` (socket id) |
| `socket.send(id, data)` | 2 | `int` (bytes) |
| `socket.receive(id, ...)` | -1 | `string` |
| `socket.sendto(id, host, port, data)` | 4 | `int` |
| `socket.recvfrom(id, ...)` | -1 | `map` |
| `socket.is_connected(id)` | 1 | `bool` |
| `socket.set_blocking(id, bool)` | 2 | `bool` |
| `socket.set_nodelay(id, bool)` | 2 | `bool` |
| `socket.http_get(url, [options])` | -1 | `map` |
| `socket.http_post(url, [options])` | -1 | `map` |
| `socket.download_file(url, [options])` | -1 | `map` |
| `socket.ping(host, [count], [timeout])` | -1 | `map` |
| `socket.get_local_ip()` | 0 | `string` |
| `socket.resolve(host)` | 1 | `string` |
| `socket.info(id)` | 1 | `map` |
| `socket.close(id)` | 1 | `bool` |
