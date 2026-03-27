/**
 * @file gc.cpp
 * @brief Garbage Collection implementation for the BuLang VM interpreter
 * 
 * This module implements a tri-color mark-and-sweep garbage collector with support for:
 * - Root marking from global variables, process privates, fiber stacks, and call frames
 * - Gray stack-based reference tracing to avoid stack overflow
 * - Object blackening based on type-specific reference patterns
 * - Automatic threshold adjustment based on allocation growth
 * 
 * The GC manages lifetime of the following object types:
 * - Struct and Class instances (user-defined types)
 * - Collections: Arrays, Maps, Buffers
 * - Native bindings: Native class and struct instances
 * - Function closures and their captured upvalues
 * 
 * Key Functions:
 * - markRoots(): Identifies all reachable objects from VM state
 * - markObject(): Marks a single object and adds it to the gray stack
 * - markValue(): Determines object type and marks accordingly
 * - traceReferences(): Processes gray stack to find all transitive references
 * - blackenObject(): Exposes references within an object for tracing
 * - sweep(): Reclaims unmarked objects and resets marks for next cycle
 * - runGC(): Orchestrates the complete GC cycle with threshold management
 * - checkGC(): Triggers collection when allocation exceeds threshold
 */
#include "interpreter.hpp"

#if defined(DEBUG_GC)
#define GC_DEBUG_LOG(...) Info(__VA_ARGS__)
#else
#define GC_DEBUG_LOG(...) ((void)0)
#endif

void Interpreter::markRoots()
{

    // OPTIMIZATION: Mark globals from globalsArray instead of HashMap
    for (size_t i = 0; i < globalsArray.size(); i++)
    {
        if (globalsArray[i].isObject())
        {
            markValue(globalsArray[i]);
        }
    }

    for (size_t i = 0; i < aliveProcesses.size(); i++)
    {
        Process *proc = aliveProcesses[i];

        // Marca privates
        for (int j = 0; j < MAX_PRIVATES; j++)
        {
            if (!proc->privates[j].isObject())
                continue;
            markValue(proc->privates[j]);
        }

        ProcessExec *fiber = proc;
        if (fiber->state != ProcessState::DEAD)
        {
            for (Value *v = fiber->stack; v < fiber->stackTop; v++)
            {
                if (!v->isObject())
                    continue;
                markValue(*v);
            }

            for (int i = 0; i < fiber->frameCount; i++)
            {
                CallFrame *frame = &fiber->frames[i];
                if (frame->closure)
                {
                    markObject((GCObject *)frame->closure);
                }
            }
        }
    }
    Upvalue *upvalue = openUpvalues;
    while (upvalue != nullptr)
    {
        markObject((GCObject *)upvalue);
        upvalue = upvalue->nextOpen;
    }
}

void Interpreter::markObject(GCObject *obj)
{
    if (__builtin_expect(obj == nullptr, 0))
        return;
    // Already marked this cycle
    if (__builtin_expect(obj->marked != 0, 0))
        return;
    obj->marked = 1;
    grayStack.push(obj);
}

void Interpreter::markValue(const Value &v)
{
    // OPTIMIZATION: Use switch for potential jump table instead of if-chain
    switch (v.type)
    {
    case ValueType::STRUCTINSTANCE:
        markObject(v.as.sInstance);
        break;
    case ValueType::CLASSINSTANCE:
        markObject(v.as.sClass);
        break;
    case ValueType::ARRAY:
        markObject(v.as.array);
        break;
    case ValueType::MAP:
        markObject(v.as.map);
        break;
    case ValueType::SET:
        markObject(v.as.set);
        break;
    case ValueType::BUFFER:
        markObject(v.as.buffer);
        break;
    case ValueType::NATIVECLASSINSTANCE:
        markObject(v.as.sClassInstance);
        break;
    case ValueType::NATIVESTRUCTINSTANCE:
        markObject(v.as.sNativeStruct);
        break;
    case ValueType::CLOSURE:
        markObject((GCObject *)v.as.closure);
        break;
    default:
        // Non-object types (INT, DOUBLE, STRING, etc.) - nothing to mark
        break;
    }
}

void Interpreter::sweep()
{
    //  Info("GC Sweep start");

    GCObject **obj = &gcObjects;
    size_t freed = 0;

    while (*obj)
    {
        GCObject *current = *obj;
        if (__builtin_expect(current->marked == 0, 0))
        {
            // Unreachable — full free
            *obj = current->next;
            freeObject(current);
            freed++;
        }
        else
        {
            // marked == 1: alive — reset for next cycle
            current->marked = 0;
            obj = &current->next;
        }
    }

    //   Info("GC Sweep freed %zu objects", freed);
}

// Immediate free: unlink from gcObjects list and free completely.
// Returns true if the object was found and freed.
bool Interpreter::freeImmediate(GCObject *target)
{
    if (!target) return false;

    // Unlink from gcObjects list
    GCObject **obj = &gcObjects;
    while (*obj)
    {
        if (*obj == target)
        {
            *obj = target->next;
            freeObject(target);
            return true;
        }
        obj = &(*obj)->next;
    }

    return false; // not found (already freed or persistent)
}

void Interpreter::freeObject(GCObject *obj)
{
    switch (obj->type)
    {
    case GCObjectType::STRUCT:
    {
        StructInstance *s = static_cast<StructInstance *>(obj);
        freeStruct(s);
        break;
    }

    case GCObjectType::CLASS:
    {
        ClassInstance *c = static_cast<ClassInstance *>(obj);
        freeClass(c);
        break;
    }

    case GCObjectType::ARRAY:
    {
        ArrayInstance *a = static_cast<ArrayInstance *>(obj);
        freeArray(a);
        break;
    }

    case GCObjectType::MAP:
    {
        MapInstance *m = static_cast<MapInstance *>(obj);
        freeMap(m);
        break;
    }

    case GCObjectType::SET:
    {
        SetInstance *s = static_cast<SetInstance *>(obj);
        freeSet(s);
        break;
    }

    case GCObjectType::BUFFER:
    {
        BufferInstance *b = static_cast<BufferInstance *>(obj);
        freeBuffer(b);
        break;
    }

    case GCObjectType::NATIVE_CLASS:
    {
        NativeClassInstance *n = static_cast<NativeClassInstance *>(obj);
        freeNativeClass(n);
        break;
    }

    case GCObjectType::NATIVE_STRUCT:
    {
        NativeStructInstance *n = static_cast<NativeStructInstance *>(obj);

        freeNativeStruct(n);
        break;
    }

    case GCObjectType::CLOSURE:
    {

        Closure *c = static_cast<Closure *>(obj);
        freeClosure(c);
        break;
    }
    case GCObjectType::UPVALUE:
    {

        Upvalue *u = static_cast<Upvalue *>(obj);
        freeUpvalue(u);
        break;
    }
    default:
        break;
    }
}

void Interpreter::checkGC()
{
    // OPTIMIZATION: Use likely/unlikely for better branch prediction
    // GC triggers are rare compared to allocations
    if (__builtin_expect(!enabledGC, 0))
        return;

    if (__builtin_expect(totalAllocated > nextGC, 0))
    {
        runGC();
    }
}

void Interpreter::blackenObject(GCObject *obj)
{
    switch (obj->type)
    {
    case GCObjectType::STRUCT:
    {
        StructInstance *s = static_cast<StructInstance *>(obj);
        for (size_t i = 0; i < s->values.size(); i++)
        {
            if (!s->values[i].isObject())
                continue;
            markValue(s->values[i]);
        }
        break;
    }

    case GCObjectType::CLASS:
    {
        ClassInstance *c = static_cast<ClassInstance *>(obj);
        for (size_t i = 0; i < c->fields.size(); i++)
        {
            if (!c->fields[i].isObject())
                continue;
            markValue(c->fields[i]);
        }
        break;
    }

    case GCObjectType::ARRAY:
    {
        ArrayInstance *a = static_cast<ArrayInstance *>(obj);
        for (size_t i = 0; i < a->values.size(); i++)
        {
            if (!a->values[i].isObject())
                continue;
            markValue(a->values[i]);
        }
        break;
    }

    case GCObjectType::MAP:
    {
        MapInstance *m = static_cast<MapInstance *>(obj);
        auto* entries = m->table.entries;
        size_t cap = m->table.capacity;
        for (size_t i = 0; i < cap; i++)
        {
            if (entries[i].state == 1)  // 1 = FILLED
            {
                if (entries[i].key.isObject())
                    markValue(entries[i].key);
                if (entries[i].value.isObject())
                    markValue(entries[i].value);
            }
        }
        break;
    }

    case GCObjectType::SET:
    {
        SetInstance *s = static_cast<SetInstance *>(obj);
        auto* entries = s->table.entries;
        size_t cap = s->table.capacity;
        for (size_t i = 0; i < cap; i++)
        {
            if (entries[i].state == 1 && entries[i].key.isObject())
            {
                markValue(entries[i].key);
            }
        }
        break;
    }

    case GCObjectType::CLOSURE:
    {

        Closure *c = static_cast<Closure *>(obj);
        for (size_t i = 0; i < c->upvalues.size(); i++)
        {
            markObject((GCObject *)c->upvalues[i]);
        }
        break;
    }
    case GCObjectType::UPVALUE:
    {

        Upvalue *u = static_cast<Upvalue *>(obj);
        if (u->closed.isObject())
        {
            markValue(u->closed);
        }
        break;
    }

    // Estes objetos não têm filhos (referências internas), ficam logo Black
    case GCObjectType::BUFFER:
    case GCObjectType::NATIVE_CLASS:
    case GCObjectType::NATIVE_STRUCT:
        break;
    }
}

void Interpreter::traceReferences()
{
    // Enquanto houver objetos na lista Gray...
    while (!grayStack.empty())
    {
        // 1. Tira o último objeto da pilha
        GCObject *obj = grayStack.back();
        grayStack.pop();
        // 2. Processa-o (transforma-o em BLACK)
        blackenObject(obj);
    }
}

void Interpreter::runGC()
{
    if (gcInProgress)
        return;
    gcInProgress = true;

#if defined(DEBUG_GC)
    size_t bytesBefore = totalAllocated;
    size_t objectsBefore = totalArrays + totalClasses + totalStructs + totalMaps + totalSets + totalBuffers + totalNativeClasses + totalNativeStructs + totalClosures + totalUpvalues;
#endif

    // OPTIMIZATION: Reserve capacity instead of clear to avoid reallocations
    grayStack.clear();
    if (grayStack.capacity() < 256) {
        grayStack.reserve(256);
    }

    markRoots();

    traceReferences();

    sweep();

    nextGC = static_cast<size_t>(totalAllocated * GC_GROWTH_FACTOR);
    if (nextGC < MIN_GC_THRESHOLD)
    {
        nextGC = MIN_GC_THRESHOLD;
    }
    if (nextGC > MAX_GC_THRESHOLD)
    {
        nextGC = MAX_GC_THRESHOLD;
    }

#if defined(DEBUG_GC)
    size_t objectCount = totalArrays + totalClasses + totalStructs + totalMaps + totalSets + totalBuffers + totalNativeClasses + totalNativeStructs + totalClosures + totalUpvalues;
    size_t bytesFreed = bytesBefore - totalAllocated;
    size_t objectsFreed = objectsBefore - objectCount;
    GC_DEBUG_LOG("GC: End - Freed %zu objects (%.2f KB). Remaining: %zu objects (%.2f KB). Next GC: %.2f KB",
                 objectsFreed, bytesFreed / 1024.0,
                 objectCount, totalAllocated / 1024.0,
                 nextGC / 1024.0);
#endif

    gcInProgress = false;

    // gcInProgress = false;
}

size_t Interpreter::countObjects() const
{
    size_t count = 0;
    GCObject *obj = gcObjects;
    while (obj)
    {
        count++;
        obj = obj->next;
    }
    obj = persistentObjects;
    while (obj)
    {
        count++;
        obj = obj->next;
    }
    return count;
}

void Interpreter::clearAllGCObjects()
{
    if (!gcObjects && !persistentObjects)
        return;

    size_t freed = 0;

    while (gcObjects)
    {
        GCObject *toFree = gcObjects;
        gcObjects = gcObjects->next;
        freeObject(toFree);
        freed++;
    }

    while (persistentObjects)
    {
        GCObject *toFree = persistentObjects;
        persistentObjects = persistentObjects->next;
        freeObject(toFree);
        freed++;
    }

    Info("Arena cleared (%zu objects freed)", freed);
}
