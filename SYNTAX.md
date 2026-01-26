# BuLang Syntax Guide

Based on demo analysis from the BuLang VM codebase.

## Variables & Data Types

### Variable Declaration
```bulang
var name = value;
var x = 10;
var text = "hello";
var arr = [1, 2, 3];
var map = {"key": "value", "num": 42};
```

### Primitives
```bulang
var num = 42;              // Integer
var decimal = 3.14;        // Double/Float
var text = "hello";        // String
var flag = true;           // Boolean
var empty = nil;           // Null/None
```

### Collections
```bulang
var arr = [1, 2, 3];       // Array
var arr = [];              // Empty array
var map = {"a": 1, "b": 2}; // Map/Dictionary
var map = {};              // Empty map
```

## Classes & Objects

### Class Definition
```bulang
class ClassName {
    var field1;
    var field2, field3;   // Multiple fields
    
    def init() {
        self.field1 = value;
        self.field2 = value;
    }
    
    def method() {
        // method body
    }
    
    def methodWithArgs(arg1, arg2) {
        return arg1 + arg2;
    }
}
```

### Class Instantiation
```bulang
var obj = ClassName();
var obj = ClassName(arg1, arg2);
var obj = ClassName("value", 42);
```

### Accessing Properties & Methods
```bulang
obj.field;
obj.field = newValue;
obj.method();
obj.method(arg);
```

### Special Methods
```bulang
def init()          // Constructor
def init(args)      // Constructor with parameters
```

## Functions

### Function Definition
```bulang
def functionName() {
    // body
}

def add(a, b) {
    return a + b;
}

def noReturn() {
    var x = 10;
}
```

### Multiple Returns
```bulang
def getPair() {
    return (1, 2);
}

var (x, y) = getPair();
var (a, b, c) = function();
```

### Function Calls
```bulang
func();
func(arg1);
func(arg1, arg2);
```

## Control Flow

### If/Elif/Else
```bulang
if (condition) {
    // code
}

if (condition) {
    // code
} else {
    // code
}

if (condition1) {
    // code
} elif (condition2) {
    // code
} elif (condition3) {
    // code
} else {
    // code
}
```

### For Loop
```bulang
for (var i = 0; i < 10; i = i + 1) {
    // code
}

for (var i = 0; i < len(arr); i = i + 1) {
    var item = arr[i];
}
```

### While Loop
```bulang
while (condition) {
    // code
}
```

### Do-While Loop
```bulang
do {
    // code (executes at least once)
} while (condition);
```

### Switch/Case (No Fallthrough - No Break Needed)
```bulang
switch (value) {
    case 1:
        print("One");
    
    case 2:
        print("Two");
    
    case 3:
        print("Three");
    
    default:
        print("Other");
}
```

Note: Cases don't fall through to the next case automatically. Once a case matches and executes, the switch exits. No `break;` statement needed.

### Infinite Loop
```bulang
loop {
    // Infinite loop (must use break to exit)
    if (shouldExit) break;
}
```

### Loop Control
```bulang
continue;   // Skip to next iteration
break;      // Exit loop
```

### For-Each Loop
```bulang
foreach (item in array) {
    print(item);
}

foreach (key in map.keys()) {
    print(key + ": " + map[key]);
}
```

## Operators

### Arithmetic
```bulang
a + b       // Addition
a - b       // Subtraction
a * b       // Multiplication
a / b       // Division
a % b       // Modulo
```

### Comparison
```bulang
a == b      // Equal
a != b      // Not equal
a < b       // Less than
a <= b      // Less than or equal
a > b       // Greater than
a >= b      // Greater than or equal
```

### Logical
```bulang
a && b      // AND
a || b      // OR
!a          // NOT
```

### Assignment
```bulang
x = value;
x = x + 1;
```

## String Operations

### String Literals
```bulang
var text = "hello";
var multiline = "hello\nworld";
```

### String Concatenation
```bulang
var msg = "Hello " + "World";
var result = "Value: " + 42;
var msg = text + (obj == obj);  // Convert to string
```

### String Methods
```bulang
text.len()              // String length
text.upper()            // Uppercase
text.lower()            // Lowercase
text.slice(start, end)  // Substring
```

## Array Operations

### Array Access
```bulang
arr[0];                 // Get element
arr[0] = newValue;      // Set element
```

### Array Methods
```bulang
arr.len()               // Length
arr.push(value);        // Add to end
arr.pop();              // Remove from end
arr.remove(index);      // Remove at index
arr.insert(index, val); // Insert at index
arr.contains(val);      // Check if contains
arr.find(val);          // Find index
arr.slice(start, end);  // Get slice
arr.concat(arr2);       // Concatenate
arr.reverse();          // Reverse
arr.first();            // First element
arr.last();             // Last element
arr.fill(value);        // Fill with value
arr.map(func);          // Map function
arr.filter(func);       // Filter with function
arr.reduce(func, init); // Reduce
```

## Map/Dictionary Operations

### Map Access
```bulang
map["key"];
map["key"] = value;
map.key;                // Dot notation if key is valid identifier
map.key = value;
```

### Map Methods
```bulang
map.len()               // Number of entries
map.keys()              // Get all keys
map.values()            // Get all values
map.has(key);           // Check if key exists
map.get(key, default);  // Get with default
map.remove(key);        // Remove key
```

## Built-in Functions

### I/O
```bulang
print(value);           // Print to stdout
print("text " + var);   // String concatenation in print
```

### Type Conversion
```bulang
str(value);             // Convert to string
int(value);             // Convert to integer
float(value);           // Convert to float
bool(value);            // Convert to boolean
```

### Math Functions
```bulang
len(arr);               // Array/String/Map length
sqrt(x);                // Square root
sin(x);                 // Sine
cos(x);                 // Cosine
abs(x);                 // Absolute value
```

### Array Functions
```bulang
len(arr);               // Array length
```

## Modules & Imports

### Require Plugin
```bulang
require "raylib";       // Load native plugin
```

### Import Module
```bulang
import math;            // Import built-in module
import raylib;
```

### Using Namespace
```bulang
using raylib;           // Use module functions without prefix
```

After `using`, functions can be called directly:
```bulang
InitWindow(800, 600, "Title");  // Instead of raylib.InitWindow()
DrawRectangle(10, 10, 100, 100, RED);
```

## Comments

### Single Line
```bulang
// This is a comment
var x = 10;  // Inline comment
```

### Multi-line (Not confirmed but may exist)
```bulang
/* This might be a block comment */
```

## Object Equality

### Identity Comparison
```bulang
obj == obj      // true (same object)
obj == other    // false (different objects)
arr == arr      // true (same array)
arr == [1,2,3]  // false (different array)
```

Equality compares by **pointer identity**, not by value:
- `nil == nil` → true
- `5 == 5` → true (primitives)
- `[1,2] == [1,2]` → false (different arrays)
- `arr1 == arr1` → true (same reference)

## Syntax Rules

### Semicolons
- Required at end of statements
- Required after variable declarations
- Required after function/class definitions
- Required after control flow

```bulang
var x = 10;
if (x > 5) {
    x = x + 1;
}
```

### Blocks
- Use `{}` for all blocks
- Classes, functions, if/else, loops all use braces

### Line Continuations
- Statements can span multiple lines if logically incomplete
- Use parentheses for complex expressions

## Example Programs

### Linked List
```bulang
class Node {
    var name;
    var url;
    var next;
    var prev;
    
    def init(name, url) {
        self.name = name;
        self.url = url;
        self.next = nil;
        self.prev = nil;
    }
}

def push(head, name, url) {
    var n = Node(name, url);
    if (head != nil) {
        head.prev = n;
        n.next = head;
    }
    return n;
}

def pop(head) {
    if (head == nil) return (nil, nil);
    return (head.next, head);
}
```

### Array Operations
```bulang
var arr = [1, 2, 3, 4, 5];
arr.remove(2);           // [1, 2, 4, 5]
arr.insert(1, 99);       // [1, 99, 2, 4, 5]
print(arr.find(99));     // 1
arr.reverse();           // [5, 4, 2, 99, 1]
var slice = arr.slice(1, 3);  // [4, 2]
```

### Simple Loop
```bulang
for (var i = 0; i < 10; i = i + 1) {
    print(i);
}

var arr = [1, 2, 3];
for (var i = 0; i < len(arr); i = i + 1) {
    var item = arr[i];
    print(item);
}
```

## Known Limitations

1. Class fields without `var` keyword may cause parser errors
2. `print()` requires `;` at end
3. Comments with special characters need to avoid parser confusion
4. String interpolation requires explicit concatenation with `+`
