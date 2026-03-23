# Math Module

```bulang
import math;
```

## Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `PI` | 3.14159265358979 | Pi |
| `E` | 2.71828182845905 | Euler's number |
| `TAU` | 6.28318530717958 | 2π |
| `SQRT2` | 1.41421356 | √2 |
| `MIN_INT` | -2147483648 | Minimum 32-bit int |
| `MAX_INT` | 2147483647 | Maximum 32-bit int |

## Basic Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `min` | `a: number`, `b: number` | `number` | Minimum of two values |
| `max` | `a: number`, `b: number` | `number` | Maximum of two values |
| `clamp` | `value`, `min`, `max` | `number` | Constrain value to range |
| `sign` | `x: number` | `-1\|0\|1` | Sign of value |

## Interpolation

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `lerp` | `start`, `end`, `t` | `double` | Linear interpolation |
| `smoothstep` | `t` OR `edge0, edge1, t` | `double` | Smooth Hermite interpolation |
| `smootherstep` | `t` OR `edge0, edge1, t` | `double` | Ken Perlin's smoother version |
| `hermite` | `v1, t1, v2, t2, amount` | `double` | Cubic Hermite interpolation |
| `catmull` | `p0, p1, p2, p3, t` | `double` | Catmull-Rom spline |

## Range Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `map` | `value, inMin, inMax, outMin, outMax` | `double` | Remap to different range |
| `repeat` | `t`, `length` | `double` | Loop value in [0, length] |
| `ping_pong` | `t`, `length` | `double` | Oscillate between 0 and length |

## Trigonometry (Extended)

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `hypot` | `dx`, `dy` | `double` | Distance √(dx² + dy²) |

## Logarithms (Extended)

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `log10` | `x: number` | `double` | Base-10 logarithm |
| `log2` | `x: number` | `double` | Base-2 logarithm |

## Hyperbolic Functions

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `sinh` | `x: number` | `double` | Hyperbolic sine |
| `cosh` | `x: number` | `double` | Hyperbolic cosine |
| `tanh` | `x: number` | `double` | Hyperbolic tangent |

## Random Numbers

| Function | Arguments | Returns | Description |
|----------|-----------|---------|-------------|
| `seed` | `value: int` | `nil` | Set RNG seed |
| `rand` | none OR `max` OR `min, max` | `double` | Random float [0,1], [0,max], or [min,max] |
| `irand` | none OR `max` OR `min, max` | `int` | Random integer [0,MAX], [0,max], or [min,max] |

## Built-in Math Keywords (No Import)

These are compiled directly to opcodes:

```bulang
sin(x)   cos(x)   tan(x)
asin(x)  acos(x)  atan(x)  atan2(y, x)
sqrt(x)  pow(x, y)  exp(x)  log(x)
abs(x)   floor(x)  ceil(x)
deg(x)   rad(x)
```

## Examples

```bulang
import math;

// Clamp health
health = math.clamp(health, 0, 100);

// Smooth animation
var t = math.smoothstep(0, 1, progress);
var pos = math.lerp(startPos, endPos, t);

// Map sensor to percentage
var percent = math.map(sensor, 0, 1023, 0, 100);

// Random enemy spawn
math.seed(12345);  // Deterministic
var x = math.irand(0, 800);
var y = math.irand(0, 600);

// Distance calculation
var dist = math.hypot(dx, dy);
```
