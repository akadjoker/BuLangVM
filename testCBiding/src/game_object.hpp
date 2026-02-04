#pragma once

#include "interpreter.hpp"
#include <cstring>
#include <cstdio>
#include <fstream>
#include <string>

// ============================================================
// ScriptComponent - NativeClass base for script inheritance
// ============================================================

struct ScriptComponentData
{
    float x;
    float y;
};

namespace ScriptComponentBindings
{

    static void *ctor(Interpreter *vm, int argc, Value *args)
    {
        ScriptComponentData *data = new ScriptComponentData();
        data->x = 0.0f;
        data->y = 0.0f;
        if (argc >= 2)
        {
            data->x = (float)args[0].asNumber();
            data->y = (float)args[1].asNumber();
        }
        return data;
    }

    static void dtor(Interpreter *vm, void *ptr)
    {
        delete static_cast<ScriptComponentData *>(ptr);
    }

    static Value getX(Interpreter *vm, void *ptr)
    {
        return vm->makeFloat(static_cast<ScriptComponentData *>(ptr)->x);
    }

    static Value getY(Interpreter *vm, void *ptr)
    {
        return vm->makeFloat(static_cast<ScriptComponentData *>(ptr)->y);
    }

    static void setX(Interpreter *vm, void *ptr, Value v)
    {
        static_cast<ScriptComponentData *>(ptr)->x = (float)v.asNumber();
    }

    static void setY(Interpreter *vm, void *ptr, Value v)
    {
        static_cast<ScriptComponentData *>(ptr)->y = (float)v.asNumber();
    }

    static void registerAll(Interpreter &vm)
    {
        NativeClassDef *sc = vm.registerNativeClass(
            "ScriptComponent",
            ctor,
            dtor,
            2,
            false
        );

        vm.addNativeProperty(sc, "x", getX, setX);
        vm.addNativeProperty(sc, "y", getY, setY);
    }

} // namespace ScriptComponentBindings

// ============================================================
// GameObject - C++ side that holds a script instance
// ============================================================

class GameObject
{
    Interpreter *vm;
    Value scriptInstance;
    bool valid;
    bool started;

public:
    GameObject(Interpreter *vm)
        : vm(vm), valid(false), started(false)
    {
        scriptInstance.type = ValueType::NIL;
    }

    bool create(const char *className, int argCount = 0, Value *args = nullptr)
    {
        scriptInstance = vm->createClassInstance(className, argCount, args);
        if (scriptInstance.isNil())
        {
            fprintf(stderr, "GameObject: class '%s' not found\n", className);
            return false;
        }
        valid = true;
        return true;
    }

    // Create without calling init() - safe during runtime (native function calls)
    bool createRaw(const char *className)
    {
        scriptInstance = vm->createClassInstanceRaw(className);
        if (scriptInstance.isNil())
        {
            fprintf(stderr, "GameObject: class '%s' not found\n", className);
            return false;
        }
        valid = true;
        return true;
    }

    void start()
    {
        if (!valid || started) return;
        vm->callMethod(scriptInstance, "start", 0, nullptr);
        started = true;
    }

    void update(float dt)
    {
        if (!valid) return;
        Value args[1];
        args[0] = vm->makeFloat(dt);
        vm->callMethod(scriptInstance, "update", 1, args);
    }

    void render()
    {
        if (!valid) return;
        vm->callMethod(scriptInstance, "render", 0, nullptr);
    }

    ScriptComponentData *getNativeData()
    {
        if (!valid || !scriptInstance.isClassInstance()) return nullptr;
        ClassInstance *inst = scriptInstance.asClassInstance();
        return static_cast<ScriptComponentData *>(inst->nativeUserData);
    }

    Value getInstance() const { return scriptInstance; }
    bool isValid() const { return valid; }
};

// ============================================================
// ScriptManager - engine core
// ============================================================
// C++ side:
//   ScriptManager engine(&vm);
//   engine.registerNatives();           // exposes createGameObject() to script
//   engine.load("bin/main.bu");         // script creates GameObjects via native call
//   while (running) engine.frame(dt);   // calls start/update/render on all objects
//
// Script side (main.bu):
//   include "player.bu";
//   include "enemy.bu";
//   var player = createGameObject("Player");
//   var enemy1 = createGameObject("Enemy");
//   var enemy2 = createGameObject("Enemy");

class ScriptManager
{
    Interpreter *vm;
    bool compiled;

    static constexpr int MAX_OBJECTS = 1024;
    GameObject *objects[MAX_OBJECTS];
    int objectCount;

    // Static pointer so the native function can access us
    static ScriptManager *instance;

    // Native function: createGameObject("ClassName") called from script
    static int nativeCreateGameObject(Interpreter *vm, int argCount, Value *args)
    {
        if (argCount < 1 || !args[0].isString())
        {
            vm->runtimeError("createGameObject expects a string (class name)");
            return 0;
        }

        const char *className = args[0].asStringChars();
        ScriptManager *self = instance;

        if (self->objectCount >= MAX_OBJECTS)
        {
            vm->runtimeError("createGameObject: too many objects (max %d)", MAX_OBJECTS);
            return 0;
        }

        // Use Raw (no init) - safe during runtime. start() handles initialization.
        GameObject *obj = new GameObject(vm);
        if (!obj->createRaw(className))
        {
            delete obj;
            return 0;
        }

        self->objects[self->objectCount++] = obj;

        // Return the script instance to the caller so they can hold a reference
        vm->push(obj->getInstance());
        return 1;
    }

public:
    ScriptManager(Interpreter *vm)
        : vm(vm), compiled(false), objectCount(0)
    {
        for (int i = 0; i < MAX_OBJECTS; i++) objects[i] = nullptr;
        instance = this;
    }

    ~ScriptManager()
    {
        for (int i = 0; i < objectCount; i++)
            delete objects[i];
    }

    // Register native functions (call BEFORE load)
    void registerNatives()
    {
        vm->registerNative("createGameObject", nativeCreateGameObject, 1);
    }

    // Load and run the main script
    bool load(const char *mainScript)
    {
        std::ifstream file(mainScript);
        if (!file.is_open())
        {
            fprintf(stderr, "ScriptManager: cannot open '%s'\n", mainScript);
            return false;
        }

        std::string source((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        if (!vm->run(source.c_str(), false))
        {
            fprintf(stderr, "ScriptManager: failed to compile '%s'\n", mainScript);
            return false;
        }

        compiled = true;
        return true;
    }

    // Call start() on all objects that haven't started yet
    void startAll()
    {
        for (int i = 0; i < objectCount; i++)
            objects[i]->start();
    }

    // Call update(dt) on all objects
    void updateAll(float dt)
    {
        for (int i = 0; i < objectCount; i++)
            objects[i]->update(dt);
    }

    // Call render() on all objects
    void renderAll()
    {
        for (int i = 0; i < objectCount; i++)
            objects[i]->render();
    }

    // One frame: start new objects + update + render
    void frame(float dt)
    {
        startAll();
        updateAll(dt);
        renderAll();
    }

    int getObjectCount() const { return objectCount; }
    GameObject *getObject(int i) { return (i >= 0 && i < objectCount) ? objects[i] : nullptr; }
    Interpreter *getVM() { return vm; }
    bool isCompiled() const { return compiled; }
};

// Static instance pointer
inline ScriptManager *ScriptManager::instance = nullptr;
