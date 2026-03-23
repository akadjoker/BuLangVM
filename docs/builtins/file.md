# File Module (Binary I/O)

```bulang
import file;
```

Low-level binary file operations for structured data.

## Handle Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `exists` | `path: string` | `bool` | Check if file exists |
| `open` | `path: string`, `mode: string` | `int` | Open file, returns handle |
| `close` | `handle: int` | `bool` | Close file handle |
| `save` | `handle: int` | `bool` | Flush buffer to disk |

**Modes:** `"r"` read, `"w"` write (truncate), `"a"` append

## Position Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `seek` | `handle: int`, `pos: int` | `bool` | Set position |
| `tell` | `handle: int` | `int` | Get current position |
| `size` | `handle: int` | `int` | Get file size |

## Write Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `write_byte` | `handle`, `value: int` | `bool` | Write 1 byte |
| `write_int` | `handle`, `value: int` | `bool` | Write 4-byte int |
| `write_float` | `handle`, `value: number` | `bool` | Write 4-byte float |
| `write_double` | `handle`, `value: number` | `bool` | Write 8-byte double |
| `write_bool` | `handle`, `value: bool` | `bool` | Write boolean |
| `write_string` | `handle`, `str: string` | `bool` | Write length-prefixed string |

## Read Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `read_byte` | `handle: int` | `int` | Read 1 byte |
| `read_int` | `handle: int` | `int` | Read 4-byte int |
| `read_float` | `handle: int` | `number` | Read 4-byte float |
| `read_double` | `handle: int` | `number` | Read 8-byte double |
| `read_bool` | `handle: int` | `bool` | Read boolean |
| `read_string` | `handle: int` | `string` | Read length-prefixed string |

## Examples

```bulang
import file;

// Write a binary record
var h = file.open("player.dat", "w");
file.write_string(h, "Player1");
file.write_int(h, 100);       // health
file.write_int(h, 50);        // score
file.write_float(h, 1.5);     // speed
file.write_bool(h, true);     // is_alive
file.save(h);
file.close(h);

// Read it back
h = file.open("player.dat", "r");
var name = file.read_string(h);
var health = file.read_int(h);
var score = file.read_int(h);
var speed = file.read_float(h);
var alive = file.read_bool(h);
file.close(h);

print(name + ": HP=" + str(health) + " Score=" + str(score));
```

## Binary Format Example

```bulang
import file;

// Save array of records
def save_records(path, records) {
    var h = file.open(path, "w");
    file.write_int(h, len(records));  // count
    for (var rec in records) {
        file.write_string(h, rec.name);
        file.write_int(h, rec.value);
    }
    file.close(h);
}

// Load array of records
def load_records(path) {
    var h = file.open(path, "r");
    var count = file.read_int(h);
    var records = [];
    for (var i = 0; i < count; i++) {
        records.push({
            name: file.read_string(h),
            value: file.read_int(h)
        });
    }
    file.close(h);
    return records;
}
```

## Notes

- Strings are stored as 4-byte length prefix + UTF-8 bytes
- Little-endian byte order
- Use `tell`/`seek` for random access
- Always `close` handles to release resources
