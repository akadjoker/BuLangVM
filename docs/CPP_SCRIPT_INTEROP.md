# C++ / Script Interoperability Guide

This document describes how to communicate between C++ and BuLang scripts in both directions.

## Table of Contents

1. [Script → C++: Using Native Classes](#script--c-using-native-classes)
2. [C++ → Script: Creating Script Instances](#c--script-creating-script-instances)
3. [Hybrid Inheritance](#hybrid-inheritance)
4. [Complete Examples](#complete-examples)

---

## Script → C++: Using Native Classes

Native Classes allow you to expose C++ functionality to BuLang scripts. Scripts can create instances, access properties, and call methods on these native objects.

### Defining a Native Class

```cpp
#include "interpreter.hpp"

// 1. Define your native data structure
struct Vector2D {
    float x;
    float y;
};

// 2. Implement the constructor
//    Returns a pointer to your native data (stored as void*)
void* Vector2D_constructor(Interpreter* vm, int argCount, Value* args) {
    Vector2D* vec = new Vector2D();
    if (argCount >= 2) {
        vec->x = (float)args[0].asNumber();
        vec->y = (float)args[1].asNumber();
    } else {
        vec->x = 0.0f;
        vec->y = 0.0f;
    }
    return vec;
}

// 3. Implement the destructor
//    Called automatically when the instance is garbage collected
void Vector2D_destructor(Interpreter* vm, void* data) {
    delete static_cast<Vector2D*>(data);
}

// 4. Define property getters
Value Vector2D_getX(Interpreter* vm, void* data) {
    return vm->makeFloat(static_cast<Vector2D*>(data)->x);
}

Value Vector2D_getY(Interpreter* vm, void* data) {
    return vm->makeFloat(static_cast<Vector2D*>(data)->y);
}

// 5. Define property setters
void Vector2D_setX(Interpreter* vm, void* data, Value value) {
    static_cast<Vector2D*>(data)->x = (float)value.asNumber();
}

void Vector2D_setY(Interpreter* vm, void* data, Value value) {
    static_cast<Vector2D*>(data)->y = (float)value.asNumber();
}

// 6. Define methods
Value Vector2D_length(Interpreter* vm, void* data, int argCount, Value* args) {
    Vector2D* vec = static_cast<Vector2D*>(data);
    float len = std::sqrt(vec->x * vec->x + vec->y * vec->y);
    return vm->makeFloat(len);
}

Value Vector2D_add(Interpreter* vm, void* data, int argCount, Value* args) {
    Vector2D* self = static_cast<Vector2D*>(data);
    
    if (argCount >= 2) {
        self->x += (float)args[0].asNumber();
        self->y += (float)args[1].asNumber();
    }
    
    return vm->makeNil();
}
```

### Registering the Native Class

```cpp
void registerNativeClasses(Interpreter& vm) {
    // Register the native class with name, constructor, destructor, and field count
    NativeClassDef* vec2 = vm.registerNativeClass("Vector2D", 
        Vector2D_constructor, 
        Vector2D_destructor, 
        2  // number of properties
    );
    
    // Add properties (name, getter, setter)
    vm.addNativeProperty(vec2, "x", Vector2D_getX, Vector2D_setX);
    vm.addNativeProperty(vec2, "y", Vector2D_getY, Vector2D_setY);
    
    // Add methods
    vm.addNativeMethod(vec2, "length", Vector2D_length);
    vm.addNativeMethod(vec2, "add", Vector2D_add);
}
```

### Using Native Classes in Script

```javascript
// Create a native instance
var pos = Vector2D(10.0, 20.0);

// Access properties
print pos.x;        // Output: 10.0
print pos.y;        // Output: 20.0

// Modify properties
pos.x = 15.0;
pos.y = 25.0;

// Call methods
print pos.length(); // Output: 29.154...

// Use in expressions
var other = Vector2D(5.0, 5.0);
pos.add(other.x, other.y);
print pos.x;        // Output: 20.0
```

---

## C++ → Script: Creating Script Instances

You can create instances of script-defined classes directly from C++ code.

### The createClassInstance API

```cpp
// Create an instance by class name
Value createClassInstance(const char* className, int argCount, Value* args);

// Create an instance from a ClassDef pointer
Value createClassInstance(ClassDef* klass, int argCount, Value* args);
```

### Example: Creating Script Instances from C++

Given this script code:

```javascript
class Enemy {
    init(id) {
        this.id = id;
        this.damage = 100;
    }
    
    attack() {
        print "Enemy " + this.id + " attacks for " + this.damage + " damage!";
    }
}
```

You can create instances from C++:

```cpp
void spawnEnemy(Interpreter& vm) {
    // Prepare constructor arguments
    Value args[1];
    args[0] = vm.makeInt(42);  // id = 42
    
    // Create the instance
    Value enemyValue = vm.createClassInstance("Enemy", 1, args);
    
    if (enemyValue.isNil()) {
        std::cerr << "Failed to create Enemy instance!" << std::endl;
        return;
    }
    
    // Access the instance
    ClassInstance* enemy = enemyValue.asClassInstance();
    
    // Read fields (by index, in order of definition)
    Value idField = enemy->fields[0];      // 'id' is first field
    Value damageField = enemy->fields[1];  // 'damage' is second
    
    std::cout << "Created enemy with id: " << idField.asInt() << std::endl;
    std::cout << "Damage: " << damageField.asInt() << std::endl;
}
```

### Working with Instance Fields

```cpp
ClassInstance* enemy = enemyValue.asClassInstance();

// Fields are stored in a Vector, indexed by order of definition
// To access by name, you need to look up the index in the class definition

ClassDef* klass = enemy->klass;
for (size_t i = 0; i < klass->fields.size(); i++) {
    if (klass->fields[i] == "damage") {
        enemy->fields[i] = vm.makeInt(200);  // Modify damage
        break;
    }
}
```

---

## Hybrid Inheritance

BuLang supports script classes inheriting from native C++ classes. This enables powerful patterns where you define base functionality in C++ and extend it in script.

### Defining Inheritance in Script

```javascript
// Entity inherits from the C++ Vector2D class
class Entity : Vector2D {
    init(x, y, name) {
        this.name = name;
    }
    
    describe() {
        print this.name + " at (" + this.x + ", " + this.y + ")";
    }
}

// Further inheritance: Enemy extends Entity (which extends Vector2D)
class Enemy : Entity {
    init(x, y) {
        super.init(x, y, "Enemy");
        this.health = 100;
    }
    
    attack() {
        print this.name + " attacks from position " + this.x + "," + this.y;
    }
}
```

### How It Works

1. **Native Properties**: When accessing `this.x` or `this.y`, the VM checks if the class (or its superclasses) inherits from a NativeClass and routes the property access to the native getter/setter.

2. **Native Methods**: When calling `this.length()`, the VM looks up the method in the native class's method table.

3. **Native User Data**: Each instance of a hybrid class contains a `nativeUserData` pointer that holds the C++ data structure (e.g., the `Vector2D` struct).

4. **Chain Inheritance**: Even with multiple levels of script inheritance (`Enemy : Entity : Vector2D`), the native functionality is preserved through the inheritance chain.

### Accessing Native Data from C++

```cpp
// Get a hybrid instance
ClassInstance* enemy = enemyValue.asClassInstance();

// Check if it has native data
if (enemy->nativeUserData != nullptr) {
    // Get the native class definition
    NativeClassDef* nativeDef = enemy->getNativeSuperclass();
    
    if (nativeDef != nullptr) {
        // Access the native data
        Vector2D* vec = static_cast<Vector2D*>(enemy->nativeUserData);
        std::cout << "Position: " << vec->x << ", " << vec->y << std::endl;
    }
}
```

---

## Complete Examples

### Example 1: Game Entity System

**C++ Side (Native Base Class):**

```cpp
struct Transform {
    float x, y;
    float rotation;
    float scaleX, scaleY;
};

void* Transform_constructor(Interpreter* vm, int argCount, Value* args) {
    Transform* t = new Transform();
    t->x = t->y = 0.0f;
    t->rotation = 0.0f;
    t->scaleX = t->scaleY = 1.0f;
    return t;
}

void Transform_destructor(Interpreter* vm, void* data) {
    delete static_cast<Transform*>(data);
}

Value Transform_getX(Interpreter* vm, void* data) {
    return vm->makeFloat(static_cast<Transform*>(data)->x);
}

void Transform_setX(Interpreter* vm, void* data, Value value) {
    static_cast<Transform*>(data)->x = (float)value.asNumber();
}

Value Transform_translate(Interpreter* vm, void* data, int argCount, Value* args) {
    Transform* t = static_cast<Transform*>(data);
    if (argCount >= 2) {
        t->x += (float)args[0].asNumber();
        t->y += (float)args[1].asNumber();
    }
    return vm->makeNil();
}

void registerTransform(Interpreter& vm) {
    NativeClassDef* cls = vm.registerNativeClass("Transform", 
        Transform_constructor, 
        Transform_destructor, 
        4  // x, y, rotation, scale
    );
    vm.addNativeProperty(cls, "x", Transform_getX, Transform_setX);
    // ... add other properties and methods
    vm.addNativeMethod(cls, "translate", Transform_translate);
}
```

**Script Side (Game Entities):**

```javascript
class GameObject : Transform {
    init(name) {
        this.name = name;
        this.active = true;
    }
    
    update(dt) {
        // Override in subclasses
    }
}

class Player : GameObject {
    init() {
        super.init("Player");
        this.speed = 200.0;
        this.health = 100;
    }
    
    update(dt) {
        if (input.left) {
            this.translate(-this.speed * dt, 0);
        }
        if (input.right) {
            this.translate(this.speed * dt, 0);
        }
    }
}

class Bullet : GameObject {
    init(x, y, vx, vy) {
        super.init("Bullet");
        this.x = x;
        this.y = y;
        this.vx = vx;
        this.vy = vy;
    }
    
    update(dt) {
        this.translate(this.vx * dt, this.vy * dt);
    }
}
```

**C++ Side (Spawning from C++):**

```cpp
void spawnBullet(Interpreter& vm, float x, float y, float vx, float vy) {
    Value args[4] = {
        vm.makeFloat(x),
        vm.makeFloat(y),
        vm.makeFloat(vx),
        vm.makeFloat(vy)
    };
    
    Value bullet = vm.createClassInstance("Bullet", 4, args);
    
    if (!bullet.isNil()) {
        // Add to game's bullet list, etc.
        gameState.bullets.push_back(bullet);
    }
}
```

### Example 2: Event System

**C++ Native Function:**

```cpp
Value dispatchEvent(Interpreter* vm, int argCount, Value* args) {
    // args[0] = event name (string)
    // args[1] = handler class name (string)
    // args[2...] = event data
    
    const char* handlerClass = args[1].asStringChars();
    
    // Create handler instance with event data
    Value handler = vm->createClassInstance(handlerClass, argCount - 2, &args[2]);
    
    // The handler's init() processes the event
    return handler;
}
```

**Script Side:**

```javascript
class ClickHandler {
    init(x, y, button) {
        print "Click at " + x + ", " + y + " with button " + button;
        this.handled = true;
    }
}

// C++ calls: dispatchEvent("click", "ClickHandler", 100, 200, 1)
```

---

## API Reference

### NativeClassDef Structure

The native class definition is created and managed by the VM. Use `registerNativeClass()` to create one.

### Function Signatures

```cpp
// Constructor: Returns pointer to native data
typedef void* (*NativeConstructor)(Interpreter* vm, int argCount, Value* args);

// Destructor: Cleans up native data (called by GC)
typedef void (*NativeDestructor)(Interpreter* vm, void* data);

// Property Getter: Returns property value
typedef Value (*NativePropertyGetter)(Interpreter* vm, void* data);

// Property Setter: Sets property value
typedef void (*NativePropertySetter)(Interpreter* vm, void* data, Value value);

// Method: Executes method logic
typedef Value (*NativeMethod)(Interpreter* vm, void* data, int argCount, Value* args);
```

### Interpreter Methods

```cpp
// Register a native class with the VM
NativeClassDef* registerNativeClass(
    const char* name,
    NativeConstructor constructor,
    NativeDestructor destructor,
    int fieldCount
);

// Add a property to a native class
void addNativeProperty(
    NativeClassDef* klass,
    const char* name,
    NativePropertyGetter getter,
    NativePropertySetter setter
);

// Add a method to a native class
void addNativeMethod(
    NativeClassDef* klass,
    const char* name,
    NativeMethod method
);

// Create a script class instance by name
Value createClassInstance(const char* className, int argCount, Value* args);

// Create a script class instance from ClassDef
Value createClassInstance(ClassDef* klass, int argCount, Value* args);

// Look up a native class by name
NativeClassDef* tryGetNativeClassDef(const std::string& name);
```

### Value Creation Methods

Use these `Interpreter` methods to create `Value` objects:

```cpp
Value makeNil();                    // Create nil value
Value makeBool(bool b);             // Create boolean value
Value makeInt(int i);               // Create integer value
Value makeUInt(uint32 i);           // Create unsigned integer value
Value makeFloat(float f);           // Create float value
Value makeDouble(double d);         // Create double value
Value makeString(const char* str);  // Create string value
Value makeString(String* str);      // Create string from String object
```

### Value Type Checking Methods

Use these `Value` instance methods:

```cpp
bool isNil() const;                 // Check if value is nil
bool isBool() const;                // Check if value is boolean
bool isInt() const;                 // Check if value is integer
bool isFloat() const;               // Check if value is float
bool isDouble() const;              // Check if value is double
bool isNumber() const;              // Check if value is any numeric type
bool isString() const;              // Check if value is a string
bool isClassInstance() const;       // Check if value is a ClassInstance
bool isNativeClassInstance() const; // Check if value is a NativeClassInstance
bool isArray() const;               // Check if value is an array
bool isMap() const;                 // Check if value is a map
```

### Value Extraction Methods

Use these `Value` instance methods:

```cpp
bool asBool() const;                     // Extract boolean
int asInt() const;                       // Extract integer
uint32 asUInt() const;                   // Extract unsigned integer
float asFloat() const;                   // Extract float
double asDouble() const;                 // Extract double
double asNumber() const;                 // Extract any numeric as double
const char* asStringChars() const;       // Extract C string
String* asString() const;                // Extract String object
ClassInstance* asClassInstance() const;  // Extract ClassInstance pointer
NativeClassInstance* asNativeClassInstance() const;  // Extract NativeClassInstance
ArrayInstance* asArray() const;          // Extract ArrayInstance pointer
MapInstance* asMap() const;              // Extract MapInstance pointer
```

---

## Best Practices

1. **Memory Management**: Always implement destructors for native classes to prevent memory leaks. The GC will call your destructor when instances are collected.

2. **Error Handling**: Check argument counts and types in native methods before accessing them:
   ```cpp
   Value myMethod(Interpreter* vm, void* data, int argCount, Value* args) {
       if (argCount < 1 || !args[0].isNumber()) {
           return vm->makeNil();  // Or throw an error
       }
       // ...
   }
   ```

3. **Minimal Native Code**: Keep native classes focused on performance-critical or system-level functionality. Let script handle game logic.

4. **Documentation**: Document the expected arguments and return values for native methods.

5. **Thread Safety**: If using fibers or multiple contexts, ensure native data is thread-safe or properly synchronized.
