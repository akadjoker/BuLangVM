# 🔀 Multi-Value Returns

Complete guide to BuLang's multi-return feature.

---

## Overview

BuLang functions and native methods can return **multiple values** efficiently using the stack-based VM. This eliminates the need for tuple overhead or container allocations.

---

## Basic Usage

### Function Definition

Return multiple values with parenthesized syntax:

```bulang
# Function returning 3 values
def divide_with_remainder(a, b) {
    return (a / b, a % b, b != 0);
}

# Call and unpack
var (quotient, remainder, is_valid) = divide_with_remainder(17, 5);

print(quotient);   # 3
print(remainder);  # 2
print(is_valid);   # true
```

### Variable Declaration with Unpacking

The `var (a, b, c) = func()` syntax automatically unpacks the returned values:

```bulang
def get_position() {
    return (100, 200, 5);
}

var (x, y, z) = get_position();
```

---

## Advanced Patterns

### Partial Unpacking

Ignore return values you don't need:

```bulang
def get_stats() {
    return (100, 50, 25, "OK", 12345);
}

# Only capture first two
var (health, mana) = get_stats();

# Rest are silently discarded
```

### Error Handling Pattern

Return success/failure as last value:

```bulang
def safe_divide(a, b) {
    if (b == 0) {
        return (0, false);
    }
    return (a / b, true);
}

var (result, success) = safe_divide(10, 2);
if (!success) {
    print("Division by zero!");
}
```

### Coordinate/Vector Functions

Natural representation for geometric operations:

```bulang
def normalize(x, y) {
    var len = math.sqrt(x*x + y*y);
    if (len == 0) return (0, 0);
    return (x/len, y/len);
}

var (nx, ny) = normalize(3, 4);
print(nx, ny);  # ~0.6 ~0.8
```

### State Queries

Efficiently return game/system state:

```bulang
def get_game_state() {
    return (
        current_level,
        player_score,
        player_hp,
        enemies_remaining,
        items_collected
    );
}

var (level, score, hp, enemies, items) = get_game_state();
```

---

## Native Function Implementation

### C++ Side

Native functions push return values and return the count:

```cpp
// Example: Return mouse position (2 values)
static int native_GetMousePosition(Interpreter *vm, int argc, Value *args)
{
    Vector2 pos = GetMousePosition();
    
    // Push both values
    vm->pushDouble(pos.x);
    vm->pushDouble(pos.y);
    
    // Return count
    return 2;
}
```

### BuLang Usage

```bulang
import raylib;

# Call native multi-return function
var (mouse_x, mouse_y) = raylib.GetMousePosition();

# Use the values
print("Mouse at: ", mouse_x, ", ", mouse_y);
```

### More Complex Example

```cpp
// Return multiple stats
static int native_GetEntityStats(Interpreter *vm, int argc, Value *args)
{
    if (argc != 1) {
        vm->runtimeError("GetEntityStats expects 1 argument");
        return 0;
    }
    
    Entity *entity = (Entity*)vm->asPointer(vm->peek(0));
    
    vm->pushDouble(entity->x);
    vm->pushDouble(entity->y);
    vm->pushDouble(entity->velocity.x);
    vm->pushDouble(entity->velocity.y);
    vm->pushBool(entity->alive);
    
    return 5;  // 5 values returned
}
```

---

## Use Cases

### 1. Physics

```bulang
def raycast(origin_x, origin_y, dir_x, dir_y, max_dist) {
    # Cast ray and return hit point + distance + object_id
    # ...
    return (hit_x, hit_y, distance, obj_id);
}

var (hx, hy, dist, hit_obj) = raycast(0, 0, 1, 0, 1000);
```

### 2. Parsing

```bulang
def parse_color_hex(hex_str) {
    # Parse "#FF00FF" -> (255, 0, 255)
    # ...
    return (r, g, b);
}

var (red, green, blue) = parse_color_hex("#FF00FF");
```

### 3. Database Queries

```bulang
def query_user(user_id) {
    # Return (name, email, is_active, last_login)
    return (name, email, is_active, last_login);
}

var (name, email, active, last) = query_user(123);
```

### 4. Collision Detection

```bulang
def check_collision(a, b) {
    # Return (collided, penetration_x, penetration_y)
    return (hit, mtv_x, mtv_y);
}

var (hit, mtv_x, mtv_y) = check_collision(player, wall);
if (hit) {
    player.x += mtv_x;
    player.y += mtv_y;
}
```

---

## Limitations & Notes

### Constraints

- **Maximum 255 values** per return (technical limit)
- Only works with `var (...)` unpacking syntax
- `return ();` is equivalent to `return;` (returns nil)
- `return (x);` is equivalent to `return x;` (single value)

### Performance

- **Zero overhead** - values stay on the stack
- No tuple allocation or copying
- No garbage collection pressure
- Faster than returning a container type

### Comparison with Alternatives

| Approach | Return Syntax | Overhead | Notes |
|----------|---------------|----------|-------|
| **Multi-Return** | `return (a, b)` | None | Native, zero-cost |
| **Array** | `return [a, b]` | Allocation | GC eligible |
| **Map** | `return {x: a, y: b}` | Allocation | GC eligible |
| **Struct** | `return Point {a, b}` | Stack alloc | Must define type |

Multi-return is **the most efficient** for temporary value bundles.

---

## Compiler Implementation Details

### Bytecode

Multi-return is compiled to `OP_RETURN_N`:

```
function foo() {
    return (a, b, c);
}

# Compiles to:
# OP_GET_LOCAL a
# OP_GET_LOCAL b
# OP_GET_LOCAL c
# OP_RETURN_N 3    <-- Multi-return opcode with count
```

### Variable Declaration

Multi-assignment unpacking:

```
var (x, y) = func();

# Compiles to:
# OP_CALL func    <-- Pushes N values
# OP_DEFINE_GLOBAL x (stack[-2])
# OP_DEFINE_GLOBAL y (stack[-1])
```

---

## Best Practices

### ✅ Do

```bulang
# Return related values together
def get_velocity(entity) {
    return (entity.vx, entity.vy);
}

# Use for error handling
def safe_operation() {
    var result, success;
    # ... do work ...
    return (result, success);
}

# Name variables clearly
var (health, mana, stamina) = player.get_resources();
```

### ❌ Don't

```bulang
# Don't return too many unrelated values
return (a, b, c, d, e, f, g, h, i, j, k);  # Confusing!

# Use map/struct instead for complex data
# Better: return {level: 1, score: 100, ...};

# Don't mix types gratuitously
return (x, y, z, "error occurred", 42, true);  # Use struct!
```

---

## See Also

- [API Reference](API.md)
- [Language Features](README.md)
- [Examples](bin/)
