# 🎨 BuLang Typed Buffers - Feature Spotlight

## Overview

BuLang v1.1 introduces **Typed Buffers** - a powerful system for working with binary data and performing zero-copy operations. This feature is essential for game development, image processing, audio manipulation, and high-performance data processing.

## Key Features

- ✅ **Type Safety**: UINT8, INT16, INT32, FLOAT support
- ✅ **Zero-Copy Operations**: Efficient slicing and views
- ✅ **File I/O**: Direct load/save from/to disk
- ✅ **Rich API**: fill(), clear(), slice(), copy(), length()
- ✅ **Method Chaining**: Fluent API for complex operations
- ✅ **Overflow Handling**: Automatic type wrapping and clamping

## Syntax

### Creating Buffers

```bulang
// Create typed buffers
var bytes = @(1024, TYPE_UINT8);      // 1KB byte buffer
var shorts = @(100, TYPE_INT16);      // 100 signed 16-bit integers
var ints = @(1000, TYPE_INT32);       // 1000 signed 32-bit integers
var floats = @(256, TYPE_FLOAT);      // 256 floats

// Load from file
var image_data = @("texture.raw", TYPE_UINT8);
```

### Indexing & Assignment

```bulang
var buffer = @(10, TYPE_UINT8);

buffer[0] = 255;
buffer[1] = 128;
buffer[2] = 64;

print(buffer[0]);  // 255
print(buffer[1]);  // 128
```

### Buffer Methods

#### fill(value)
Fill entire buffer with a single value.

```bulang
var buffer = @(100, TYPE_FLOAT);
buffer.fill(3.14159);  // All elements = PI

print(buffer[0]);   // 3.14159
print(buffer[99]);  // 3.14159
```

#### clear()
Zero out all buffer elements.

```bulang
var buffer = @(50, TYPE_INT32);
buffer.fill(42);
buffer.clear();  // All elements = 0

print(buffer[0]);  // 0
```

#### length()
Get buffer size in elements.

```bulang
var buffer = @(256, TYPE_UINT8);
print(buffer.length());  // 256
```

#### slice(start, end)
Create a view into a subset of the buffer.

```bulang
var original = @(10, TYPE_INT32);
original.fill(100);

// Create view of [2, 3, 4]
var view = original.slice(2, 5);
print(view.length());  // 3
print(view[0]);        // 100 (same as original[2])

// Modifying the view affects the original
view.fill(999);
print(original[2]);    // 999
```

#### copy(dst_offset, src, src_offset, count)
Copy data between buffers.

```bulang
var src = @(5, TYPE_UINT8);
var dst = @(10, TYPE_UINT8);

// Fill source
for (var i = 0; i < 5; i = i + 1) {
    src[i] = i * 10;
}

// Copy src[0:5] to dst[3:8]
dst.copy(3, src, 0, 5);

// Result: dst = [0, 0, 0, 0, 10, 20, 30, 40, 0, 0]
```

#### save(filename)
Save buffer contents to file (binary).

```bulang
var pixels = @(1920 * 1080 * 4, TYPE_UINT8);
pixels.fill(128);  // Gray image
pixels.save("output.raw");
```

## Type System

### Supported Types

| Type         | Size    | Range                     | Use Case           |
|--------------|---------|---------------------------|--------------------|
| TYPE_UINT8   | 1 byte  | 0 to 255                  | Pixels, bytes      |
| TYPE_INT16   | 2 bytes | -32,768 to 32,767         | Audio samples      |
| TYPE_INT32   | 4 bytes | -2.1B to 2.1B             | Large integers     |
| TYPE_FLOAT   | 4 bytes | ±3.4e38 (7 digits)        | Coordinates, math  |

### Overflow Behavior

Buffers handle overflow automatically:

```bulang
var b8 = @(4, TYPE_UINT8);

b8[0] = 255;   // OK
b8[1] = -1;    // Wraps to 255 (unsigned)
b8[2] = 256;   // Wraps to 0 (overflow)
b8[3] = 300;   // Wraps to 44 (300 % 256)

print(b8[0]);  // 255
print(b8[1]);  // 255
print(b8[2]);  // 0
print(b8[3]);  // 44
```

## Use Cases

### 1. Image Processing

```bulang
// Create RGB image (100x100 pixels)
var width = 100;
var height = 100;
var image = @(width * height * 3, TYPE_UINT8);

// Fill with gradient
for (var y = 0; y < height; y = y + 1) {
    for (var x = 0; x < width; x = x + 1) {
        var i = (y * width + x) * 3;
        image[i + 0] = x * 2;      // R
        image[i + 1] = y * 2;      // G
        image[i + 2] = 128;        // B (constant)
    }
}

// Save as raw RGB
image.save("gradient.raw");

// Convert with ImageMagick:
// $ convert -size 100x100 -depth 8 rgb:gradient.raw output.png
```

### 2. Audio Buffer Processing

```bulang
// 44.1kHz mono audio, 1 second
var sample_rate = 44100;
var audio = @(sample_rate, TYPE_INT16);

// Generate sine wave at 440 Hz (A4 note)
var frequency = 440.0;
for (var i = 0; i < sample_rate; i = i + 1) {
    var t = i / sample_rate;
    var sample = sin(2.0 * 3.14159 * frequency * t) * 32767.0;
    audio[i] = sample;
}

audio.save("tone_440hz.raw");
```

### 3. Particle System Data

```bulang
// Particle system with 10,000 particles
// Each particle: x, y, vx, vy (4 floats)
var num_particles = 10000;
var particles = @(num_particles * 4, TYPE_FLOAT);

// Initialize particles
for (var i = 0; i < num_particles; i = i + 1) {
    var base = i * 4;
    particles[base + 0] = random() * 800;  // x
    particles[base + 1] = random() * 600;  // y
    particles[base + 2] = (random() - 0.5) * 2;  // vx
    particles[base + 3] = (random() - 0.5) * 2;  // vy
}
```

### 4. Network Packet Serialization

```bulang
// Create packet: [type (1 byte), id (4 bytes), data (N bytes)]
def create_packet(type, id, data) {
    var packet = @(5 + data.length(), TYPE_UINT8);
    
    packet[0] = type;
    packet[1] = (id >> 24) & 0xFF;
    packet[2] = (id >> 16) & 0xFF;
    packet[3] = (id >> 8) & 0xFF;
    packet[4] = id & 0xFF;
    
    packet.copy(5, data, 0, data.length());
    return packet;
}
```

### 5. Texture Atlas Generation

```bulang
// Combine multiple small textures into one large atlas
var atlas_size = 1024;
var atlas = @(atlas_size * atlas_size * 4, TYPE_UINT8);
atlas.clear();

// Load textures
var tex1 = @("tex1.raw", TYPE_UINT8);
var tex2 = @("tex2.raw", TYPE_UINT8);

// Copy to atlas (simplified - would need 2D logic)
atlas.copy(0, tex1, 0, tex1.length());
atlas.copy(256 * 256 * 4, tex2, 0, tex2.length());

atlas.save("atlas.raw");
```

## Method Chaining

Buffers support fluent API for chaining operations:

```bulang
var result = @(100, TYPE_FLOAT)
    .fill(0.0)           // Clear
    .slice(10, 50)       // Take middle section
    .fill(3.14);         // Fill with PI

print(result.length());  // 40
print(result[0]);        // 3.14
```

## Performance Characteristics

- **Memory**: Contiguous allocation, cache-friendly
- **Indexing**: O(1) direct memory access
- **Slicing**: O(1) view creation (no copy)
- **Copy**: O(n) memcpy performance
- **Fill**: O(n) optimized fill
- **File I/O**: Direct binary read/write, no serialization overhead

## Comparison with Arrays

| Feature           | Arrays            | Typed Buffers       |
|-------------------|-------------------|---------------------|
| Type              | Dynamic (any)     | Fixed (type-safe)   |
| Memory            | Boxed values      | Raw binary          |
| Performance       | Good              | Excellent           |
| Size overhead     | ~24 bytes/elem    | Type-dependent      |
| File I/O          | JSON              | Binary (fast)       |
| Interop           | High-level        | Zero-copy FFI       |

## Best Practices

1. **Use the right type**: Match buffer type to your data
2. **Avoid unnecessary copies**: Use slices for views
3. **Batch operations**: Use fill() instead of loops when possible
4. **Clear after use**: Call clear() to zero sensitive data
5. **Validate sizes**: Check buffer.length() before indexing
6. **Use save/load**: Binary I/O is much faster than text

## Real-World Example: Image Thumbnail Generator

```bulang
// Load original image (1920x1080 RGB)
var original = @("photo.raw", TYPE_UINT8);
var orig_w = 1920;
var orig_h = 1080;

// Create thumbnail (320x180 RGB)
var thumb_w = 320;
var thumb_h = 180;
var thumbnail = @(thumb_w * thumb_h * 3, TYPE_UINT8);

// Simple downsampling (6x6 blocks -> 1 pixel)
var scale = 6;
for (var y = 0; y < thumb_h; y = y + 1) {
    for (var x = 0; x < thumb_w; x = x + 1) {
        // Sample from original
        var src_x = x * scale;
        var src_y = y * scale;
        var src_idx = (src_y * orig_w + src_x) * 3;
        
        // Copy pixel
        var dst_idx = (y * thumb_w + x) * 3;
        thumbnail[dst_idx + 0] = original[src_idx + 0];  // R
        thumbnail[dst_idx + 1] = original[src_idx + 1];  // G
        thumbnail[dst_idx + 2] = original[src_idx + 2];  // B
    }
}

thumbnail.save("thumbnail.raw");
print("Thumbnail generated: 320x180");
```

## Future Enhancements (Roadmap)

- [ ] **Buffer pools**: Reusable buffer allocation
- [ ] **SIMD operations**: Vectorized fill/copy
- [ ] **Compression**: Built-in zlib/deflate
- [ ] **View arithmetic**: Add, multiply buffers element-wise
- [ ] **Typed accessors**: Read multi-byte values (uint16_at, float32_at)
- [ ] **Endianness control**: Big/little endian conversions

---

## Conclusion

Typed Buffers bring BuLang into the realm of serious systems programming while maintaining its game development focus. Whether you're processing pixels, generating audio, or handling network packets, typed buffers provide the performance and control you need.

**Examples**: See `examples.json` → "Typed Buffers (Binary Data)"  
**Playground**: https://bulang.dev/playground.html  
**GitHub**: https://github.com/akadjoker/bulang
