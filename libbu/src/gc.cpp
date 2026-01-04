#include "interpreter.hpp"

FORCE_INLINE void Interpreter::markRoots()
{
    // stack da VM

    Fiber *fiber = currentFiber;
    if (fiber)
    {
        for (Value *v = fiber->stack; v < fiber->stackTop; v++)
        {
            markValue(*v);
        }
    }

    globals.forEach([this](String *key, Value val)
                    { markValue(val); });
}

void Interpreter::markArray(ArrayInstance *a)
{
    if (!a || a->marked)
        return;
    a->marked = 1;
    for (size_t i = 0; i < a->values.size(); i++)
    {
        markValue(a->values[i]);
    }
}

void Interpreter::markClass(ClassInstance *c)
{
    if (!c || c->marked)
        return;

    c->marked = 1;
    for (size_t i = 0; i < c->fields.size(); i++)
    {
        markValue(c->fields[i]);
    }
}

void Interpreter::markStruct(StructInstance *s)
{
    if (!s || s->marked)
        return;
    s->marked = 1;
    for (size_t i = 0; i < s->values.size(); i++)
    {
        markValue(s->values[i]);
    }
}

void Interpreter::markMap(MapInstance *m)
{
    if (!m || m->marked)
        return;
    m->marked = 1;

    m->table.forEach([this](String *key, Value val)
                     { markValue(val); });
}

void Interpreter::markNativeClass(NativeClassInstance *n)
{
    if (!n || n->marked)
        return;
    n->marked = 1;
}

void Interpreter::markNativeStruct(NativeStructInstance *n)
{
    if (!n || n->marked)
        return;
    n->marked = 1;
}

void Interpreter::sweepArrays()
{

    for (size_t i = 0; i < arrayInstances.size();)
    {
        ArrayInstance *a = arrayInstances[i];
        if (a->marked == 0)
        {
            freeArray(a);
            arrayInstances[i] = arrayInstances.back();
            arrayInstances.pop();
        }
        else
        {
            a->marked = 0;
            ++i;
        }
    }
}

void Interpreter::sweepStructs()
{

    for (size_t i = 0; i < structInstances.size();)
    {
        StructInstance *s = structInstances[i];
        if (s->marked == 0)
        {
            freeStruct(s);
            structInstances[i] = structInstances.back();
            structInstances.pop();
        }
        else
        {
            s->marked = 0;
            ++i;
        }
    }
}

void Interpreter::sweepClasses()
{

    for (size_t i = 0; i < classInstances.size();)
    {
        ClassInstance *c = classInstances[i];
        if (c->marked == 0)
        {
            freeClass(c);
            classInstances[i] = classInstances.back();
            classInstances.pop();
        }
        else
        {
            ++i;
        }
    }
}

void Interpreter::sweepMaps()
{

    for (size_t i = 0; i < mapInstances.size();)
    {
        MapInstance *m = mapInstances[i];
        if (m->marked == 0)
        {
            freeMap(m);
            mapInstances[i] = mapInstances.back();
            mapInstances.pop();
        }
        else
        {
            m->marked = 0;
            ++i;
        }
    }
}

void Interpreter::sweepNativeClasses()
{
    for (size_t i = 0; i < nativeInstances.size();)
    {
        NativeClassInstance *n = nativeInstances[i];

        if (n->marked == 0)
        {
            freeNativeClass(n);
            nativeInstances[i] = nativeInstances.back();
            nativeInstances.pop();
        }
        else
        {
            ++i;
        }
    }
}

void Interpreter::sweepNativeStructs()
{
    for (size_t i = 0; i < nativeStructInstances.size();)
    {
        NativeStructInstance *n = nativeStructInstances[i];

        if (n->marked == 0)
        {
            freeNativeStruct(n);
            nativeStructInstances[i] = nativeStructInstances.back();
            nativeStructInstances.pop();
        }
        else
        {
            n->marked = 0;
            ++i;
        }
    }
}

void Interpreter::markValue(const Value &v)
{
    if (v.isClassInstance())
        markClass(v.as.sClass);
    else if (v.isArray())
        markArray(v.as.array);
    else if (v.isStruct())
        markStruct(v.as.sInstance);
    else if (v.isMap())
        markMap(v.as.map);
    else if (v.isNativeClass())
        markNativeClass(v.as.sClassInstance);
    else if (v.isNativeStructInstance())
        markNativeStruct(v.as.sNativeStruct);
}

void Interpreter::checkGC()
{
    if(!enbaledGC) return;
    
    if (totalAllocated > nextGC)
    {
        runGC();
    }
}

void Interpreter::runGC()
{
    gcInProgress = true;

    if (totalAllocated<=nextGC) 
    {
        gcInProgress = false;
        return;
    }

    size_t before = totalAllocated;

    for (size_t i = 0; i < arrayInstances.size(); ++i)
    {
        arrayInstances[i]->marked = 0;
    }
    for (size_t i = 0; i < structInstances.size(); ++i)
    {
        structInstances[i]->marked = 0;
    }
    for (size_t i = 0; i < classInstances.size(); ++i)
    {
        classInstances[i]->marked = 0;
    }
    for (size_t i = 0; i < mapInstances.size(); ++i)
    {
        mapInstances[i]->marked = 0;
    }
    for (size_t i = 0; i < nativeInstances.size(); ++i)
    {
        nativeInstances[i]->marked = 0;
    }
    for (size_t i = 0; i < nativeStructInstances.size(); ++i)
    {
        nativeStructInstances[i]->marked = 0;
    }

    markRoots();
    sweepArrays();
    sweepMaps();
    sweepStructs();
    sweepClasses();
    sweepNativeClasses();
    sweepNativeStructs();

    nextGC = totalAllocated * 2;
    if (nextGC < 1024)
        nextGC = 1024; // Mínimo 1KB

     Info("GC: %zu -> %zu bytes (next at %zu)", before, totalAllocated, nextGC);
     gcInProgress = false;
}
