# Zip Module

```bulang
import zip;
```

ZIP archive operations: read, extract, and create.

## Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `list` | `zipPath: string` | `array` | List files in archive |
| `read` | `zipPath: string`, `filename: string` | `string` | Read file as text |
| `read_buffer` | `zipPath: string`, `filename: string` | `buffer` | Read file as binary buffer |
| `extract` | `zipPath: string`, `destDir: string` | `bool` | Extract entire archive |
| `create` | `zipPath: string`, `files: array` | `bool` | Create new archive |

## List Entry Format

`list()` returns array of maps:

```bulang
[
    {
        name: string,      // Filename in archive
        size: int,         // Uncompressed size
        compressed: int    // Compressed size
    },
    ...
]
```

## Examples

### List Archive Contents

```bulang
import zip;

var entries = zip.list("archive.zip");
for (var entry in entries) {
    print(entry.name + " (" + str(entry.size) + " bytes)");
}
```

### Read File from Archive

```bulang
import zip;

// Read as text
var readme = zip.read("archive.zip", "README.md");
print(readme);

// Read as binary buffer
var data = zip.read_buffer("archive.zip", "image.png");
print("Size: " + str(data.length()) + " bytes");
```

### Extract Archive

```bulang
import zip;

// Extract to directory
if (zip.extract("archive.zip", "./output/")) {
    print("Extracted successfully");
} else {
    print("Extraction failed");
}
```

### Create Archive

```bulang
import zip;

// Create from array of {name, content}
var files = [
    {name: "hello.txt", content: "Hello, World!"},
    {name: "data/config.json", content: '{"key": "value"}'},
    {name: "notes.md", content: "# Notes\n\nSome text."}
];

if (zip.create("output.zip", files)) {
    print("Archive created");
}
```

## Common Patterns

```bulang
import zip;
import fs;
import path;

// Read config from zip
def load_zip_config(zipPath) {
    var content = zip.read(zipPath, "config.json");
    return json.parse(content);
}

// Check if file exists in archive
def zip_contains(zipPath, filename) {
    var entries = zip.list(zipPath);
    for (var entry in entries) {
        if (entry.name == filename) {
            return true;
        }
    }
    return false;
}

// Archive a directory
def archive_directory(dirPath, zipPath) {
    var files = [];
    var entries = fs.list(dirPath);
    for (var entry in entries) {
        var fullPath = path.join(dirPath, entry);
        if (path.isfile(fullPath)) {
            files.push({
                name: entry,
                content: fs.read(fullPath)
            });
        }
    }
    return zip.create(zipPath, files);
}
```

## Notes

- `read()` assumes UTF-8 text encoding
- `read_buffer()` for binary files
- `extract()` preserves directory structure
- `create()` supports nested paths (e.g., "dir/file.txt")
- Uses miniz compression library
