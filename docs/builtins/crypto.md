# Crypto Modules

Cryptographic utilities: encoding, hashing, and UUIDs.

These are separate modules (not submodules of a single `crypto` module):

```bulang
import base64;
import hex;
import hash;
import uuid;
```

## Base64 Module

```bulang
import base64;
```

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `base64.encode` | `data: string` | `string` | Encode to Base64 |
| `base64.decode` | `encoded: string` | `string` | Decode from Base64 |

## Hex Module

```bulang
import hex;
```

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `hex.encode` | `data: string` | `string` | Encode to hex string |
| `hex.decode` | `hexStr: string` | `string` | Decode from hex string |

## Hash Module

```bulang
import hash;
```

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `hash.md5` | `data: string` | `string` | MD5 hash (hex) |
| `hash.sha256` | `data: string` | `string` | SHA-256 hash (hex) |

## UUID Module

```bulang
import uuid;
```

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `uuid.v4` | none | `string` | Generate random UUID v4 |

## Examples

```bulang
import base64;
import hex;
import hash;
import uuid;

// Base64 encoding
var encoded = base64.encode("Hello, World!");
print(encoded);  // "SGVsbG8sIFdvcmxkIQ=="

var decoded = base64.decode(encoded);
print(decoded);  // "Hello, World!"

// Hex encoding
var hexStr = hex.encode("ABC");
print(hexStr);  // "414243"

var original = hex.decode(hexStr);
print(original);  // "ABC"

// Hashing
var md5 = hash.md5("password123");
print("MD5: " + md5);

var sha = hash.sha256("password123");
print("SHA256: " + sha);

// Generate UUID
var id = uuid.v4();
print("UUID: " + id);
// -> "550e8400-e29b-41d4-a716-446655440000"
```

## Common Patterns

```bulang
import hash;
import uuid;
import base64;
import time;

// Simple checksum validation
def verify_data(data, expected_hash) {
    return hash.sha256(data) == expected_hash;
}

// Generate unique identifiers
def create_record() {
    return {
        id: uuid.v4(),
        created: time.now()
    };
}

// Encode binary data for transport
def encode_binary(data) {
    return base64.encode(data);
}
```

## Notes

- MD5 is for checksums only, NOT secure for passwords
- SHA-256 is cryptographically secure
- UUID v4 uses random generation
- Hex encoding doubles string length
- Base64 encoding increases size by ~33%
