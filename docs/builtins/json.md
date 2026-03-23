# JSON Module

```bulang
import json;
```

JSON serialization and deserialization.

## Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `parse` | `jsonString: string` | `any` | Parse JSON string to value |
| `stringify` | `value: any` | `string` | Convert value to JSON string |

## Examples

```bulang
import json;

// Parse JSON string (note: escape inner quotes)
var data = json.parse("{\"name\": \"Alice\", \"age\": 30}");
print(data.name);  // "Alice"
print(data.age);   // 30

// Parse array
var items = json.parse("[1, 2, 3, \"four\"]");
print(items[0]);  // 1
print(items[3]);  // "four"

// Stringify map
var obj = {
    title: "Test",
    count: 42,
    active: true
};
var jsonStr = json.stringify(obj);
print(jsonStr);
// -> {"title":"Test","count":42,"active":true}

// Stringify array
var arr = [1, 2, {nested: "value"}];
print(json.stringify(arr));
// -> [1,2,{"nested":"value"}]
```

## File Operations

```bulang
import json;
import fs;

// Load JSON file
def load_json(path) {
    var content = fs.read(path);
    return json.parse(content);
}

// Save JSON file
def save_json(path, data) {
    var content = json.stringify(data);
    fs.write(path, content);
}

// Usage
var config = load_json("config.json");
config.version = 2;
save_json("config.json", config);
```

## Supported Types

| BuLang Type | JSON Type |
|-------------|-----------|
| `nil` | `null` |
| `bool` | `boolean` |
| `int`, `double` | `number` |
| `string` | `string` |
| `array` | `array` |
| `map` | `object` |

## Notes

- Objects with class instances serialize their public fields
- Functions and native types cannot be serialized
- Parse errors return `nil` or throw error
- Circular references are not supported
