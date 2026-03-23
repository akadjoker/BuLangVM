# Filesystem Module

```bulang
import fs;
```

High-level filesystem operations for text files and directories.

## File Operations

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `read` | `path: string` | `string` | Read entire file as text |
| `write` | `path: string`, `content: string` | `bool` | Write text to file (overwrites) |
| `append` | `path: string`, `content: string` | `bool` | Append text to file |
| `remove` | `path: string` | `bool` | Delete file |

## Directory Operations

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `mkdir` | `path: string` | `bool` | Create directory |
| `rmdir` | `path: string` | `bool` | Remove empty directory |
| `list` | `path: string` | `array` | List directory entries |

## File Information

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `stat` | `path: string` | `map` | Get file/dir metadata |

### Stat Returns

```bulang
{
    size: int,       // File size in bytes
    mtime: int,      // Modification time (Unix timestamp)
    isfile: bool,    // Is regular file
    isdir: bool,     // Is directory
    mode: int        // File mode/attributes
}
```

## Examples

```bulang
import fs;

// Read and write text files
var content = fs.read("config.txt");
fs.write("output.txt", "Hello, World!");
fs.append("log.txt", "New entry\n");

// Create directory structure
fs.mkdir("data");
fs.mkdir("data/cache");

// List directory
var entries = fs.list(".");
for (var entry in entries) {
    print(entry);
}

// Get file info
var info = fs.stat("myfile.txt");
print("Size: " + str(info.size) + " bytes");
print("Modified: " + str(info.mtime));
if (info.isfile) print("It's a file");
if (info.isdir) print("It's a directory");

// Clean up
fs.remove("temp.txt");
fs.rmdir("empty_dir");
```

## Config File Example

```bulang
import fs;
import json;

def load_config(path) {
    var st = fs.stat(path);
    if (st == nil || !st.isfile) {
        return {};
    }
    return json.parse(fs.read(path));
}

def save_config(path, config) {
    fs.write(path, json.stringify(config));
}

// Usage
var config = load_config("settings.json");
config.volume = 80;
save_config("settings.json", config);
```

## Notes

- Use `fs` for text file operations (UTF-8)
- Use `file` module for binary/structured data
- `list()` returns filenames only, not full paths
- `mkdir()` fails if directory exists
- `rmdir()` fails if directory is not empty
