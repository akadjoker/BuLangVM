# Time Module

```bulang
import time;
```

## Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `now` | none | `int` | Current Unix timestamp (seconds) |
| `now_ms` | none | `int` | Current timestamp (milliseconds) |
| `current` | none | `double` | High-resolution time (nanoseconds as seconds.fraction) |
| `sleep` | `seconds: number` | `nil` | Sleep for N seconds |
| `sleep_ms` | `ms: int` | `nil` | Sleep for N milliseconds |
| `date` | `timestamp?: int` | `map` | Decompose timestamp into components |
| `ftime` | `timestamp: int`, `format?: string` | `string` | Format timestamp as string |
| `parse` | `dateStr: string`, `format: string` | `int` | Parse string to timestamp |
| `diff` | `t1: int`, `t2: int` | `int` | Difference in seconds (t1 - t2) |

## date() Returns

```bulang
{
    year: int,      // e.g., 2025
    month: int,     // 1-12
    day: int,       // 1-31
    hour: int,      // 0-23
    minute: int,    // 0-59
    second: int,    // 0-59
    weekday: int,   // 0-6 (Sunday=0)
    yearday: int    // 0-365
}
```

## Format Specifiers

Used with `ftime()` and `parse()`:

| Specifier | Description |
|-----------|-------------|
| `%Y` | Year (4 digits) |
| `%m` | Month (01-12) |
| `%d` | Day (01-31) |
| `%H` | Hour (00-23) |
| `%M` | Minute (00-59) |
| `%S` | Second (00-59) |

## Examples

```bulang
import time;

// Get current time
var ts = time.now();
var ms = time.now_ms();

// High-resolution timing
var start = time.current();
// ... some operation
var elapsed = time.current() - start;
print("Elapsed: " + str(elapsed) + "s");

// Get date components
var d = time.date();           // current time
print(d.year);                 // 2025
print(d.month);                // 3
print(d.day);                  // 23

var d2 = time.date(1705312200);  // specific timestamp
print(d2.year);                   // 2024

// Format timestamp as string
var formatted = time.ftime(ts);  // default: "%Y-%m-%d %H:%M:%S"
print(formatted);                 // "2025-03-23 14:30:00"

var custom = time.ftime(ts, "%Y-%m-%d");
print(custom);                    // "2025-03-23"

// Parse date string
var timestamp = time.parse("2025-01-15", "%Y-%m-%d");

// Calculate difference
var diff = time.diff(ts1, ts2);
print("Difference: " + str(diff) + " seconds");

// Sleep
print("Waiting...");
time.sleep(2);        // 2 seconds
time.sleep_ms(500);   // 500 milliseconds
print("Done!");
```

## Timing Example

```bulang
import time;

def benchmark(callable, iterations) {
    var start = time.current();
    for (var i = 0; i < iterations; i++) {
        callable();
    }
    var elapsed = time.current() - start;
    print("Time: " + str(elapsed) + "s");
    return elapsed;
}
```
