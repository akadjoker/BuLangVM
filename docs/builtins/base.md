# Core/Base Functions

Global functions available without imports.

## Output Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `format` | `template: string`, `...args` | `string` | Format string with `{}` placeholders |
| `write` | `template: string`, `...args` | `nil` | Print formatted string (no newline) |
| `print` | `...args` | `nil` | Print values (keyword, compiled to opcode) |

## Input Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `input` | `[prompt: string]` | `string` | Read line from stdin |

## Type Conversion

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `str` | `value: any` | `string` | Convert to string |
| `int` | `value: any` | `int` | Convert to integer |
| `real` | `value: any` | `double` | Convert to double |
| `char` | `code: int` | `string` | Convert codepoint to UTF-8 character |

## Type Inspection

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `classname` | `instance: any` | `string` | Get class/struct name |
| `typeid` | `type_or_instance` | `int` | Get encoded type ID |
| `len` | `value: any` | `int` | Length of array/string/map/buffer (keyword) |

## System Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `ticks` | `delta: number` | `nil` | Update VM with delta time |
| `clock` | none | `double` | High-precision CPU time (keyword) |

## Debug Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `print_stack` | `[label: string]` | `nil` | Print VM stack trace |
| `_gc` | none | `nil` | Force garbage collection |

## GC Probe Functions (Debug)

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `_gcProbeReset` | none | `nil` | Reset GC probe stats |
| `_gcProbePersistentCtorCount` | none | `int` | Persistent probe constructor count |
| `_gcProbePersistentDtorCount` | none | `int` | Persistent probe destructor count |
| `_gcProbeEphemeralCtorCount` | none | `int` | Ephemeral probe constructor count |
| `_gcProbeEphemeralDtorCount` | none | `int` | Ephemeral probe destructor count |

## GC Probe Classes (Testing)

### GcPersistentProbe

```bulang
var probe = GcPersistentProbe(42);  // Create with ID
```

| Method | Returns | Description |
|--------|---------|-------------|
| `id()` | `int` | Get probe ID |
| `isPersistent()` | `bool` | Always true |
| `destroyPayload()` | `bool` | Destroy internal payload |
| `isPayloadAlive()` | `bool` | Check if payload alive |

### GcEphemeralProbe

Same API as `GcPersistentProbe`, but `isPersistent()` returns false.

## Examples

```bulang
// String formatting
var msg = format("Player {} has {} HP", name, health);
write("Loading... ");
print("Done!");

// Input
var name = input("Enter name: ");

// Type conversion
var n = int("42");        // 42
var s = str(3.14);        // "3.14"
var f = real("2.5");      // 2.5

// Type inspection
print(classname(player)); // "Player"
print(len([1, 2, 3]));    // 3

// Timing
var start = clock();
// ... work ...
var elapsed = clock() - start;
```
