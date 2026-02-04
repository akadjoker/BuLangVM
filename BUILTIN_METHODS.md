# 📦 BuLang Built-in Methods

Documentation for native methods available on built-in types.

---

## 📋 Table of Contents

- [String Methods](#-string-methods)
- [Array Methods](#-array-methods)
- [Map Methods](#-map-methods)
- [Buffer Methods](#-buffer-methods)

---

## 📝 String Methods

Methods available on string instances.

### `length()`
Get string length.

```bulang
text = "Hello";
len = text.length();  # 5
```

### `upper()`
Convert to uppercase.

```bulang
text = "hello".upper();  # "HELLO"
```

### `lower()`
Convert to lowercase.

```bulang
text = "HELLO".lower();  # "hello"
```

### `concat(str)`
Concatenate with another string.

```bulang
result = "Hello".concat(" World");  # "Hello World"
```

### `sub(start, end)`
Extract substring from start to end index.

```bulang
text = "Hello World";
sub = text.sub(0, 5);  # "Hello"
sub = text.sub(6, 11); # "World"
```

### `replace(old, new)`
Replace all occurrences of old with new.

```bulang
text = "Hello World";
result = text.replace("World", "BuLang");  # "Hello BuLang"
```

### `at(index)`
Get character at index.

```bulang
text = "Hello";
char = text.at(0);  # "H"
char = text.at(4);  # "o"
```

### `contains(substring)`
Check if string contains substring.

```bulang
text = "Hello World";
has = text.contains("World");  # true
has = text.contains("Bye");    # false
```

### `trim()`
Remove leading and trailing whitespace.

```bulang
text = "  Hello  ";
clean = text.trim();  # "Hello"
```

### `startsWith(prefix)`
Check if string starts with prefix.

```bulang
text = "Hello World";
starts = text.startsWith("Hello");  # true
starts = text.startsWith("World");  # false
```

### `endsWith(suffix)`
Check if string ends with suffix.

```bulang
text = "Hello World";
ends = text.endsWith("World");  # true
ends = text.endsWith("Hello");  # false
```

### `indexOf(substring, [startIndex])`
Find first occurrence of substring.

```bulang
text = "Hello World Hello";
index = text.indexOf("Hello");     # 0
index = text.indexOf("Hello", 1);  # 12
index = text.indexOf("Bye");       # -1 (not found)
```

### `repeat(count)`
Repeat string count times.

```bulang
text = "Ha".repeat(3);  # "HaHaHa"
line = "-".repeat(10);  # "----------"
```

### `split(delimiter)`
Split string into array.

```bulang
# Split by delimiter
text = "a,b,c";
parts = text.split(",");  # ["a", "b", "c"]

# Split each character
text = "abc";
chars = text.split("");   # ["a", "b", "c"]

# Split by space
text = "Hello World";
words = text.split(" ");  # ["Hello", "World"]
```

---

## 📚 Array Methods

Methods available on array instances.

### `push(item)`
Add item to end. Returns array (chainable).

```bulang
arr = [1, 2, 3];
arr.push(4);  # [1, 2, 3, 4]

# Chainable
arr.push(5).push(6);  # [1, 2, 3, 4, 5, 6]
```

### `pop()`
Remove and return last item.

```bulang
arr = [1, 2, 3];
last = arr.pop();  # 3
# arr is now [1, 2]
```

### `back()`
Get last item without removing.

```bulang
arr = [1, 2, 3];
last = arr.back();  # 3
# arr is still [1, 2, 3]
```

### `length()`
Get array length.

```bulang
arr = [1, 2, 3];
len = arr.length();  # 3
```

### `clear()`
Remove all elements.

```bulang
arr = [1, 2, 3];
arr.clear();  # []
```

### `remove(index)`
Remove element at index.

```bulang
arr = [10, 20, 30];
arr.remove(1);  # [10, 30]
```

### `insert(index, item)`
Insert item at index.

```bulang
arr = [1, 3, 4];
arr.insert(1, 2);  # [1, 2, 3, 4]
```

### `find(value)`
Find first index of value. Returns -1 if not found.

```bulang
arr = [10, 20, 30, 20];
index = arr.find(20);  # 1
index = arr.find(99);  # -1
```

### `contains(value)`
Check if array contains value.

```bulang
arr = [1, 2, 3];
has = arr.contains(2);  # true
has = arr.contains(5);  # false
```

### `reverse()`
Reverse array in-place. Returns array (chainable).

```bulang
arr = [1, 2, 3];
arr.reverse();  # [3, 2, 1]
```

### `slice(start, end)`
Extract sub-array from start to end.

```bulang
arr = [1, 2, 3, 4, 5];
sub = arr.slice(1, 4);  # [2, 3, 4]

# Negative indices
sub = arr.slice(-2, -1);  # [4]
```

### `concat(otherArray)`
Concatenate arrays (creates new array).

```bulang
arr1 = [1, 2];
arr2 = [3, 4];
result = arr1.concat(arr2);  # [1, 2, 3, 4]
```

### `first()`
Get first element (or nil if empty).

```bulang
arr = [10, 20, 30];
first = arr.first();  # 10

empty = [];
first = empty.first();  # nil
```

### `last()`
Get last element (or nil if empty).

```bulang
arr = [10, 20, 30];
last = arr.last();  # 30

empty = [];
last = empty.last();  # nil
```

### `fill(value)`
Fill all elements with value.

```bulang
arr = [0, 0, 0];
arr.fill(5);  # [5, 5, 5]
```

---

## 🗺️ Map Methods

Methods available on map (dictionary) instances.

### `has(key)`
Check if map contains key.

```bulang
map = {name: "Alice", age: 30};
exists = map.has("name");  # true
exists = map.has("city");  # false
```

### `remove(key)`
Remove key-value pair.

```bulang
map = {name: "Alice", age: 30};
map.remove("age");
# map is now {name: "Alice"}
```

### `clear()`
Remove all key-value pairs.

```bulang
map = {a: 1, b: 2};
map.clear();  # {}
```

### `length()`
Get number of key-value pairs.

```bulang
map = {a: 1, b: 2, c: 3};
len = map.length();  # 3
```

### `keys()`
Get array of all keys.

```bulang
map = {name: "Alice", age: 30, city: "NYC"};
keys = map.keys();  # ["name", "age", "city"]
```

### `values()`
Get array of all values.

```bulang
map = {name: "Alice", age: 30, city: "NYC"};
vals = map.values();  # ["Alice", 30, "NYC"]
```

### Example Usage

```bulang
# Build map
player = {};
player["name"] = "Hero";
player["hp"] = 100;
player["level"] = 5;

# Check key
if player.has("hp") {
    print("HP: {}", player["hp"]);
}

# Iterate keys
for key in player.keys() {
    print("{}: {}", key, player[key]);
}

# Get all values
stats = player.values();
```

---

## 🔧 Buffer Methods

Binary buffer manipulation with cursor-based I/O.

### Creating Buffers

```bulang
# Create typed buffer
buf = Buffer(100, BUFFER_UINT8);   # 100 bytes
buf = Buffer(50, BUFFER_INT32);    # 50 integers (200 bytes)
buf = Buffer(25, BUFFER_FLOAT);    # 25 floats (100 bytes)
```

Buffer types:
- `BUFFER_UINT8` - unsigned byte
- `BUFFER_INT16` - 16-bit signed integer
- `BUFFER_UINT16` - 16-bit unsigned integer
- `BUFFER_INT32` - 32-bit signed integer
- `BUFFER_UINT32` - 32-bit unsigned integer
- `BUFFER_FLOAT` - 32-bit float
- `BUFFER_DOUBLE` - 64-bit double

### Basic Operations

#### `length()`
Get buffer capacity (number of elements).

```bulang
buf = Buffer(100, BUFFER_UINT8);
len = buf.length();  # 100
```

#### `clear()`
Zero all bytes.

```bulang
buf.clear();  # All bytes set to 0
```

#### `fill(value)`
Fill buffer with value.

```bulang
buf = Buffer(10, BUFFER_UINT8);
buf.fill(255);  # All bytes = 0xFF

buf = Buffer(10, BUFFER_FLOAT);
buf.fill(3.14);  # All floats = 3.14
```

### Write Methods (Cursor-based)

All write methods advance the cursor and return the buffer (chainable).

#### `writeByte(value)`
Write single byte.

```bulang
buf = Buffer(10, BUFFER_UINT8);
buf.writeByte(0xFF);
```

#### `writeShort(value)` / `writeUShort(value)`
Write 16-bit integer.

```bulang
buf.writeShort(-1000);   # int16
buf.writeUShort(65535);  # uint16
```

#### `writeInt(value)` / `writeUInt(value)`
Write 32-bit integer.

```bulang
buf.writeInt(-100000);
buf.writeUInt(4294967295);
```

#### `writeFloat(value)`
Write 32-bit float.

```bulang
buf.writeFloat(3.14159);
```

#### `writeDouble(value)`
Write 64-bit double.

```bulang
buf.writeDouble(3.141592653589793);
```

#### `writeString(str)`
Write string bytes (no null terminator).

```bulang
buf = Buffer(100, BUFFER_UINT8);
buf.writeString("Hello");  # Writes 5 bytes
```

### Read Methods (Cursor-based)

All read methods advance the cursor.

#### `readByte()`
Read single byte.

```bulang
value = buf.readByte();  # 0-255
```

#### `readShort()` / `readUShort()`
Read 16-bit integer.

```bulang
value = buf.readShort();   # int16
value = buf.readUShort();  # uint16
```

#### `readInt()` / `readUInt()`
Read 32-bit integer.

```bulang
value = buf.readInt();   # int32
value = buf.readUInt();  # uint32 (as double)
```

#### `readFloat()`
Read 32-bit float.

```bulang
value = buf.readFloat();
```

#### `readDouble()`
Read 64-bit double.

```bulang
value = buf.readDouble();
```

#### `readString(length)`
Read string of specified length.

```bulang
text = buf.readString(5);  # Read 5 bytes as string
```

### Cursor Control

#### `seek(position)`
Move cursor to position.

```bulang
buf.seek(0);    # Start
buf.seek(50);   # Middle
```

#### `tell()`
Get current cursor position.

```bulang
pos = buf.tell();
```

#### `rewind()`
Reset cursor to start.

```bulang
buf.rewind();  # cursor = 0
```

#### `skip(bytes)`
Move cursor forward/backward.

```bulang
buf.skip(10);   # Forward 10 bytes
buf.skip(-5);   # Back 5 bytes
```

#### `remaining()`
Get bytes remaining from cursor to end.

```bulang
left = buf.remaining();
```

### Advanced Operations

#### `copy(dstOffset, srcBuffer, srcOffset, count)`
Copy data from another buffer.

```bulang
src = Buffer(100, BUFFER_UINT8);
dst = Buffer(100, BUFFER_UINT8);

# Copy 50 elements from src[10:] to dst[0:]
dst.copy(0, src, 10, 50);
```

#### `slice(start, end)`
Create new buffer with copied data.

```bulang
buf = Buffer(100, BUFFER_UINT8);
# ... fill data ...

# Extract middle portion
sub = buf.slice(25, 75);  # New buffer with 50 elements
```

#### `save(filename)`
Save buffer to file (raw binary).

```bulang
buf = Buffer(100, BUFFER_FLOAT);
# ... fill data ...
buf.save("data.bin");
```

### Complete Example

```bulang
import file;

# Create buffer
buf = Buffer(100, BUFFER_UINT8);

# Write structured data
buf.writeInt(1234);           # Magic number
buf.writeString("SAVE01");    # Version
buf.writeFloat(99.5);         # Player health
buf.writeInt(42);             # Player level

# Save to file
buf.save("savegame.dat");

# Read back
buf2 = Buffer(100, BUFFER_UINT8);
# Load from file somehow...

buf2.rewind();
magic = buf2.readInt();       # 1234
version = buf2.readString(6); # "SAVE01"
health = buf2.readFloat();    # 99.5
level = buf2.readInt();       # 42
```

### Binary Protocol Example

```bulang
# Network packet
buf = Buffer(256, BUFFER_UINT8);

# Header
buf.writeByte(0x01);          # Packet type
buf.writeUShort(payload_len); # Length

# Payload
buf.writeInt(player_id);
buf.writeFloat(pos_x);
buf.writeFloat(pos_y);
buf.writeFloat(pos_z);

# Send via socket
socket.send(sock, buf.slice(0, buf.tell()));
```

---

## 🔄 Method Chaining

Many methods return the object itself, allowing chaining:

### String Chaining
```bulang
result = "  hello world  "
    .trim()
    .upper()
    .replace("WORLD", "BULANG");
# "HELLO BULANG"
```

### Array Chaining
```bulang
arr = []
    .push(1)
    .push(2)
    .push(3)
    .reverse();
# [3, 2, 1]
```

### Buffer Chaining
```bulang
buf = Buffer(100, BUFFER_UINT8)
    .writeInt(42)
    .writeFloat(3.14)
    .writeString("Hello")
    .rewind();
```

---

## 💡 Tips & Best Practices

### Strings
- Strings are **immutable** - methods return new strings
- Use `concat()` or `+` for concatenation
- `split("")` splits each character
- Negative indices not supported (use `length()` for calculations)

### Arrays
- Arrays are **mutable** - most methods modify in-place
- Use `slice()` to create copies
- `find()` returns -1 if not found
- Empty arrays: `first()` and `last()` return `nil`

### Maps
- Keys must be **strings**
- Use `has()` before accessing to avoid errors
- `keys()` and `values()` return arrays
- Iteration: `for key in map.keys()`

### Buffers
- **Choose correct type** for your data
- Always check `remaining()` before reading
- Use `rewind()` to read data multiple times
- **Cursor auto-advances** on read/write
- Use `slice()` for partial copies
- **Binary files**: Use `save()` for raw data

---

## ⚠️ Common Pitfalls

### Index Out of Bounds
```bulang
# ❌ Wrong
arr = [1, 2, 3];
arr.remove(10);  # Error!

# ✅ Correct
if index < arr.length() {
    arr.remove(index);
}
```

### Empty Collections
```bulang
# ❌ Wrong
arr = [];
last = arr.pop();  # Warning!

# ✅ Correct
if arr.length() > 0 {
    last = arr.pop();
}
```

### Buffer Cursor
```bulang
# ❌ Wrong
buf.writeInt(42);
buf.writeInt(99);
value = buf.readInt();  # Reads garbage (cursor at end)

# ✅ Correct
buf.writeInt(42);
buf.writeInt(99);
buf.rewind();
value = buf.readInt();  # 42
```

### String Immutability
```bulang
# ❌ Wrong (doesn't modify original)
text = "hello";
text.upper();
print(text);  # Still "hello"

# ✅ Correct
text = "hello";
text = text.upper();
print(text);  # "HELLO"
```

---

## 📖 See Also

- [Core API Reference](API.md) - Built-in modules and functions
- [Language Guide](https://bulang.netlify.app/docs.html) - Syntax and semantics
- [Examples](examples/) - Real-world usage patterns