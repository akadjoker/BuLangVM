# Typed Arrays (Native Classes)

Typed arrays for efficient numerical data storage. All arrays share the same API.

## Classes

| Class | Element Type | Size |
|-------|--------------|------|
| `Uint8Array` | unsigned 8-bit | 1 byte |
| `Int16Array` | signed 16-bit | 2 bytes |
| `Uint16Array` | unsigned 16-bit | 2 bytes |
| `Int32Array` | signed 32-bit | 4 bytes |
| `Uint32Array` | unsigned 32-bit | 4 bytes |
| `Float32Array` | 32-bit float | 4 bytes |
| `Float64Array` | 64-bit double | 8 bytes |

## Global Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `TYPE_UINT8` | 0 | Buffer type for unsigned 8-bit |
| `TYPE_INT16` | 1 | Buffer type for signed 16-bit |
| `TYPE_UINT16` | 2 | Buffer type for unsigned 16-bit |
| `TYPE_INT32` | 3 | Buffer type for signed 32-bit |
| `TYPE_UINT32` | 4 | Buffer type for unsigned 32-bit |
| `TYPE_FLOAT` | 5 | Buffer type for 32-bit float |
| `TYPE_DOUBLE` | 6 | Buffer type for 64-bit double |

## Constructor

```bulang
var arr = Uint8Array(size);           // Preallocate capacity
var arr = Float32Array([1.0, 2.0]);   // From array
var arr = Int32Array(buffer);          // From buffer
var arr = Float64Array(otherTyped);   // From another typed array
```

**Arguments:**
- `size: int` - Preallocate capacity (count = 0)
- `array: Array` - Initialize from array of numbers
- `buffer: Buffer` - Initialize from buffer
- `typedArray: TypedArray` - Copy from another typed array

## Methods

| Method | Arguments | Returns | Description |
|--------|-----------|---------|-------------|
| `add` | `value: number` OR `array` OR `buffer` OR `typedArray` | `int` | Append element(s), returns new count |
| `get` | `index: int` | `number` | Get element at index |
| `set` | `index: int`, `value: number` | `int` | Set element at index, returns index |
| `length` | none | `int` | Get element count |
| `capacity` | none | `int` | Get allocated capacity |
| `byteLength` | none | `int` | Get total byte size (count × element size) |
| `byteCapacity` | none | `int` | Get allocated byte capacity |
| `ptr` | none | `pointer` | Get raw data pointer (for FFI) |
| `clear` | none | `int` | Clear all elements (count = 0) |
| `reserve` | `count: int` | `int` | Ensure capacity for count elements |
| `pack` | none | `int` | Shrink capacity to fit count |
| `toBuffer` | none | `Buffer` | Convert to native buffer |

## Properties (Read-only)

| Property | Type | Description |
|----------|------|-------------|
| `length` | `int` | Element count |
| `capacity` | `int` | Allocated capacity |
| `byteLength` | `int` | Total bytes used |
| `ptr` | `pointer` | Raw data pointer |

## Example

```bulang
var positions = Float32Array(1000);  // Preallocate for 1000 floats

// Add elements
positions.add(100.5);
positions.add(200.3);
positions.add([1.0, 2.0, 3.0]);  // Add from array

// Access
print(positions.get(0));  // 100.5
positions.set(0, 150.0);

// Info
print(positions.length());      // 5
print(positions.byteLength());  // 20 (5 × 4 bytes)

// Convert to buffer for file I/O
var buf = positions.toBuffer();
```

## Memory Management

- `reserve(n)` - Preallocate for n elements to avoid reallocations
- `pack()` - Shrink to fit after removing elements
- `clear()` - Reset count to 0 (keeps capacity)

## Use Cases

- Game vertex data
- Audio samples
- Image pixels
- Binary protocols
- Interop with native code (via `ptr`)
