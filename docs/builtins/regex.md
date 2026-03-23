# Regex Module

```bulang
import regex;
```

Regular expression matching and manipulation.

## Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `match` | `pattern: string`, `text: string` | `bool` | Test if pattern matches entire string |
| `search` | `pattern: string`, `text: string` | `bool` | Test if pattern matches anywhere |
| `replace` | `pattern: string`, `replacement: string`, `text: string` | `string` | Replace all matches |
| `findall` | `pattern: string`, `text: string` | `array` | Find all matches |

## Examples

```bulang
import regex;

// Test if matches entire string
var isEmail = regex.match("^[\\w.]+@[\\w.]+$", "test@example.com");
print(isEmail);  // true

// Test if pattern found anywhere  
var found = regex.search("\\d+\\.\\d+", "Price: $42.99");
print(found);  // true

// Replace all occurrences
var clean = regex.replace("\\s+", " ", "hello   world");
print(clean);  // "hello world"

// Find all matches
var numbers = regex.findall("\\d+", "a1b2c3d4");
for (var n in numbers) {
    print(n);  // "1", "2", "3", "4"
}
```

## Pattern Syntax

Standard regex syntax:

| Pattern | Description |
|---------|-------------|
| `.` | Any character |
| `*` | Zero or more |
| `+` | One or more |
| `?` | Zero or one |
| `^` | Start of string |
| `$` | End of string |
| `[]` | Character class |
| `[^]` | Negated class |
| `\d` | Digit |
| `\w` | Word character |
| `\s` | Whitespace |
| `\b` | Word boundary |
| `()` | Grouping |
| `\|` | Alternation |

## Common Patterns

```bulang
import regex;

// Validate email
def is_valid_email(email) {
    return regex.match(email, "^[\\w.+-]+@[\\w.-]+\\.[a-zA-Z]{2,}$");
}

// Extract URLs
def extract_urls(text) {
    return regex.findall(text, "https?://[\\w./\\-?=&]+");
}

// Clean whitespace
def normalize_spaces(text) {
    return regex.replace(text, "\\s+", " ");
}

// Parse key=value
def parse_config_line(line) {
    var result = regex.search(line, "^(\\w+)=(.*)$");
    if (result != nil) {
        return {key: result.match};  // Note: groups not yet supported
    }
    return nil;
}
```

## Notes

- Uses ECMAScript regex flavor
- Double-escape backslashes in strings: `"\\d+"` for `\d+`
- `match()` tests entire string (implicit `^...$`)
- `search()` finds first match anywhere
- `replace()` replaces ALL occurrences
