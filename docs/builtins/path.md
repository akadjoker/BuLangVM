# Path Module

```bulang
import path;
```

Path manipulation utilities.

## Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `join` | `...parts: string` | `string` | Join path components |
| `normalize` | `path: string` | `string` | Normalize path (resolve `.`, `..`) |
| `basename` | `path: string` | `string` | Get filename from path (with extension) |
| `dirname` | `path: string` | `string` | Get directory from path |
| `filename`* | `path: string` | `string` | Get filename without extension |
| `extname` | `path: string` | `string` | Get file extension (with dot) |
| `extension`* | `path: string` | `string` | Alias for extname |
| `exists` | `path: string` | `bool` | Check if path exists |
| `isdir` | `path: string` | `bool` | Check if path is directory |
| `isfile` | `path: string` | `bool` | Check if path is file |

*Available in BuGL repository version

## Examples

```bulang
import path;

// Join paths
var fullPath = path.join("/home", "user", "docs", "file.txt");
// -> "/home/user/docs/file.txt"

// Parse path components
var file = "/home/user/project/main.bu";

print(path.basename(file));  // "main.bu"
print(path.dirname(file));   // "/home/user/project"
print(path.filename(file));  // "main"
print(path.extname(file));   // ".bu"

// Normalize messy paths
var clean = path.normalize("/home/user/../user/./docs");
// -> "/home/user/docs"

// Check existence
if (path.exists("config.json")) {
    if (path.isfile("config.json")) {
        print("Config file found");
    }
}

// Check directory
if (path.isdir("data")) {
    print("Data directory exists");
}
```

## Common Patterns

```bulang
import path;
import fs;

// Process files with specific extension
def find_scripts(dir) {
    var scripts = [];
    var entries = fs.list(dir);
    for (var entry in entries) {
        var full = path.join(dir, entry);
        if (path.isfile(full) && path.extname(entry) == ".bu") {
            scripts.push(full);
        }
    }
    return scripts;
}

// Change extension
def change_ext(filepath, newExt) {
    var dir = path.dirname(filepath);
    var name = path.filename(filepath);
    return path.join(dir, name + newExt);
}

var output = change_ext("script.bu", ".buc");
// -> "script.buc"
```

## Notes

- Uses platform-appropriate separators
- `join` handles multiple arguments
- `extname` includes the dot (e.g., ".txt")
- `basename` and `dirname` work like POSIX equivalents
