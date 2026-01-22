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
    double x;
    double y;
};

// 2. Implement the constructor
//    Returns a pointer to your native data (stored as void*)
void* Vector2D_constructor(Interpreter* vm, int argCount, Value* args) {
    Vector2D* vec = new Vector2D();
    vec->x = (argCount > 0) ? AS_NUMBER(args[0]) : 0.0;
    vec->y = (argCount > 1) ? AS_NUMBER(args[1]) : 0.0;
    return vec;
}

// 3. Implement the destructor
//    Called automatically when the instance is garbage collected
void Vector2D_destructor(void* data) {
    delete static_cast<Vector2D*>(data);
}

// 4. Define property getters
Value Vector2D_getX(void* data) {
    return NUMBER_VAL(static_cast<Vector2D*>(data)->x);
}

Value Vector2D_getY(void* data) {
    return NUMBER_VAL(static_cast<Vector2D*>(data)->y);
}

// 5. Define property setters
void Vector2D_setX(void* data, Value value) {
    static_cast<Vector2D*>(data)->x = AS_NUMBER(value);
}

void Vector2D_setY(void* data, Value value) {
    static_cast<Vector2D*>(data)->y = AS_NUMBER(value);
}

// 6. Define methods
Value Vector2D_length(Interpreter* vm, void* data, int argCount, Value* args) {
    Vector2D* vec = static_cast<Vector2D*>(data);
    double len = std::sqrt(vec->x * vec->x + vec->y * vec->y);
    return NUMBER_VAL(len);
}

Value Vector2D_add(Interpreter* vm, void* data, int argCount, Value* args) {
    Vector2D* self = static_cast<Vector2D*>(data);
    
    if (argCount < 1 || !IS_NATIVE_INSTANCE(args[0])) {
        return NIL_VAL;
    }
    
    NativeInstance* other = AS_NATIVE_INSTANCE(args[0]);
    Vector2D* otherVec = static_cast<Vector2D*>(other->data);
    
    self->x += otherVec->x;
    self->y += otherVec->y;
    
    return NIL_VAL;
}
```

### Registering the Native Class

```cpp
void registerNativeClasses(Interpreter& vm) {
    // Create the native class definition
    NativeClassDef* vectorClass = new NativeClassDef();
    vectorClass->name = "Vector2D";
    vectorClass->constructor = Vector2D_constructor;
    vectorClass->destructor = Vector2D_destructor;
    
    // Register properties (name, getter, setter)
    vectorClass->properties["x"] = {Vector2D_getX, Vector2D_setX};
    vectorClass->properties["y"] = {Vector2D_getY, Vector2D_setY};
    
    // Register methods
    vectorClass->methods["length"] = Vector2D_length;
    vectorClass->methods["add"] = Vector2D_add;
    
    // Register with the VM
    vm.registerNativeClass(vectorClass);
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
pos.add(other);
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
    args[0] = NUMBER_VAL(42);  // id = 42
    
    // Create the instance
    Value enemyValue = vm.createClassInstance("Enemy", 1, args);
    
    if (IS_NIL(enemyValue)) {
        std::cerr << "Failed to create Enemy instance!" << std::endl;
        return;
    }
    
    // Access the instance
    ClassInstance* enemy = AS_CLASS_INSTANCE(enemyValue);
    
    // Read fields
    Value idField = enemy->fields[0];      // Assuming 'id' is first field
    Value damageField = enemy->fields[1];  // Assuming 'damage' is second
    
    std::cout << "Created enemy with id: " << AS_NUMBER(idField) << std::endl;
    std::cout << "Damage: " << AS_NUMBER(damageField) << std::endl;
}
```

### Working with Instance Fields

```cpp
ClassInstance* enemy = AS_CLASS_INSTANCE(enemyValue);

// Fields are stored in a Vector, indexed by order of definition
// To access by name, you need to look up the index in the class definition

ClassDef* klass = enemy->klass;
for (size_t i = 0; i < klass->fields.size(); i++) {
    if (klass->fields[i] == "damage") {
        enemy->fields[i] = NUMBER_VAL(200);  // Modify damage
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
        super.init(x, y);   // Calls Vector2D constructor (currently implicit)
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
ClassInstance* enemy = AS_CLASS_INSTANCE(enemyValue);

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
    double x, y;
    double rotation;
    double scaleX, scaleY;
};

void* Transform_constructor(Interpreter* vm, int argCount, Value* args) {
    Transform* t = new Transform();
    t->x = t->y = 0.0;
    t->rotation = 0.0;
    t->scaleX = t->scaleY = 1.0;
    return t;
}

void Transform_destructor(void* data) {
    delete static_cast<Transform*>(data);
}

Value Transform_translate(Interpreter* vm, void* data, int argCount, Value* args) {
    Transform* t = static_cast<Transform*>(data);
    if (argCount >= 2) {
        t->x += AS_NUMBER(args[0]);
        t->y += AS_NUMBER(args[1]);
    }
    return NIL_VAL;
}

void registerTransform(Interpreter& vm) {
    NativeClassDef* cls = new NativeClassDef();
    cls->name = "Transform";
    cls->constructor = Transform_constructor;
    cls->destructor = Transform_destructor;
    cls->properties["x"] = {/* getter */, /* setter */};
    cls->properties["y"] = {/* getter */, /* setter */};
    cls->properties["rotation"] = {/* getter */, /* setter */};
    cls->methods["translate"] = Transform_translate;
    vm.registerNativeClass(cls);
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
void spawnBullet(Interpreter& vm, double x, double y, double vx, double vy) {
    Value args[4] = {
        NUMBER_VAL(x),
        NUMBER_VAL(y),
        NUMBER_VAL(vx),
        NUMBER_VAL(vy)
    };
    
    Value bullet = vm.createClassInstance("Bullet", 4, args);
    
    if (!IS_NIL(bullet)) {
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
    
    const char* handlerClass = AS_CSTRING(args[1]);
    
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

```cpp
struct NativeClassDef {
    const char* name;                                    // Class name
    NativeConstructor constructor;                       // Constructor function
    NativeDestructor destructor;                         // Destructor function
    std::unordered_map<std::string, NativeProperty> properties;  // Properties
    std::unordered_map<std::string, NativeMethod> methods;       // Methods
};
```

### Function Signatures

```cpp
// Constructor: Returns pointer to native data
typedef void* (*NativeConstructor)(Interpreter* vm, int argCount, Value* args);

// Destructor: Cleans up native data
typedef void (*NativeDestructor)(void* data);

// Property Getter: Returns property value
typedef Value (*NativePropertyGetter)(void* data);

// Property Setter: Sets property value
typedef void (*NativePropertySetter)(void* data, Value value);

// Method: Executes method logic
typedef Value (*NativeMethod)(Interpreter* vm, void* data, int argCount, Value* args);

// Property structure
struct NativeProperty {
    NativePropertyGetter getter;
    NativePropertySetter setter;
};
```

### Interpreter Methods

```cpp
// Register a native class with the VM
void registerNativeClass(NativeClassDef* klass);

// Create a script class instance by name
Value createClassInstance(const char* className, int argCount, Value* args);

// Create a script class instance from ClassDef
Value createClassInstance(ClassDef* klass, int argCount, Value* args);

// Look up a native class by name
NativeClassDef* tryGetNativeClassDef(const std::string& name);
```

### Value Type Checking Macros

```cpp
IS_NIL(value)              // Check if value is nil
IS_NUMBER(value)           // Check if value is a number
IS_STRING(value)           // Check if value is a string
IS_NATIVE_INSTANCE(value)  // Check if value is a NativeInstance
IS_CLASS_INSTANCE(value)   // Check if value is a ClassInstance

AS_NUMBER(value)           // Extract number from value
AS_STRING(value)           // Extract ObjString* from value
AS_CSTRING(value)          // Extract const char* from value
AS_NATIVE_INSTANCE(value)  // Extract NativeInstance* from value
AS_CLASS_INSTANCE(value)   // Extract ClassInstance* from value
```

---

## Best Practices

1. **Memory Management**: Always implement destructors for native classes to prevent memory leaks.

2. **Error Handling**: Check argument counts and types in native methods before accessing them.

3. **Minimal Native Code**: Keep native classes focused on performance-critical or system-level functionality. Let script handle game logic.

4. **Documentation**: Document the expected arguments and return values for native methods.

5. **Thread Safety**: If using fibers or multiple contexts, ensure native data is thread-safe or properly synchronized.
