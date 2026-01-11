#include "interpreter.hpp"

FORCE_INLINE void Interpreter::markRoots()
{
    // stack da VM
    //  for (auto* proc : aliveProcesses) {
    //     for (int f = 0; f < proc->totalFibers; f++) {
    //         Fiber* fiber = &proc->fibers[f];
    //         for (Value* v = fiber->stack; v < fiber->stackTop; v++) {
    //             markValue(*v);
    //         }
    //     }
    // }

    
}

void Interpreter::markArray(ArrayInstance *a)
{

   // Info("Sweeping array");
    for (size_t j = 0; j < a->values.size(); j++)
    {
        Value v = a->values[j];
        markForFree(v);
 
    }
    a->values.clear();
 
}

void Interpreter::markClass(ClassInstance *c)
{
    c->marked = 1;
    for (size_t i = 0; i < c->fields.size(); i++)
    {
        markForFree(c->fields[i]);
    }
    c->fields.clear();
}

void Interpreter::markStruct(StructInstance *s)
{

    s->marked = 1;
    for (size_t i = 0; i < s->values.size(); i++)
    {
        markForFree(s->values[i]);
    }
    s->values.clear();
}

void Interpreter::markMap(MapInstance *m)
{

    m->marked = 1;
    m->table.forEach([this](String *key, Value val)
                     { markForFree(val); });
    m->table.destroy();
}

void Interpreter::markNativeClass(NativeClassInstance *n)
{
    n->marked = 1;
}

void Interpreter::markNativeStruct(NativeStructInstance *n)
{
    n->marked = 1;
}

void Interpreter::markBuffer(BufferInstance *b)
{
    if (!b || b->marked)
        return;
    b->marked = 1;
}

 
 

void Interpreter::markForFree(const Value &v)
{
    if (v.isStructInstance())
    {
        StructInstance *s = v.asStructInstance();

        if (s && s->marked == 0)
        {
            s->marked = 1;

            for (size_t i = 0; i < s->values.size(); i++)
            {
                markForFree(s->values[i]);
            }
        }
    }
    else if (v.isArray())
    {
        ArrayInstance *a = v.asArray();
        if (a && a->marked == 0)
        {
            a->marked = 1;
            for (size_t i = 0; i < a->values.size(); i++)
            {
                markForFree(a->values[i]);
            }
        }
    }
    else if (v.isMap())
    {
        MapInstance *m = v.asMap();
        if (m && m->marked == 0)
        {
            m->marked = 1;
            m->table.forEach([this](String *key, Value val)
                             { markForFree(val); });
        }
    }
    else if (v.isClassInstance())
    {
        ClassInstance *c = v.asClassInstance();
        if (c && c->marked == 0)
        {
            c->marked = 1;

            for (size_t i = 0; i < c->fields.size(); i++)
            {
                markForFree(c->fields[i]);
            }
        }
    } else if (v.isNativeStructInstance())
    {
        NativeStructInstance *ns = v.asNativeStructInstance();
        if (ns && ns->marked == 0)
        {
            ns->marked = 1;

        }

    }
    else if (v.isBuffer())
    {
        BufferInstance *b = v.asBuffer();
        if (b && b->marked == 0)
        {
            b->marked = 1;
        }
    }
}

void Interpreter::sweepArrays()
{

   // Info("Sweeping arrays (%zu)", arrayInstances.size());

    for (size_t i = 0; i < arrayInstances.size();)
    {
        ArrayInstance *a = arrayInstances[i];
        if (a->marked == 1)
        {
            markArray(a);
            arrayInstances[i] = arrayInstances.back();
            arrayInstances.pop();
            freeArray(a);
        }
        else
        {
            a->marked = 0;
            ++i;
        }
    }

   // Info("Sweeping arrays (%zu)", arrayInstances.size());
}

void Interpreter::sweepStructs()
{
    //Info("Sweeping structs (%zu)", structInstances.size());
    size_t i = 0;
    while (i < structInstances.size())
    {
        StructInstance *s = structInstances[i];
        if (s->marked == 1)
        {
     //                   Info("Sweeping  Struct address: %p", (void*)s);
            markStruct(s);
            structInstances[i] = structInstances.back();
            structInstances.pop();
            freeStruct(s);
        }
        else
        {
            i++;
        }
    }

   // Info("Sweeping structs (%zu)", structInstances.size());
}

void Interpreter::sweepClasses()
{

    size_t i = 0;
    while (i < classInstances.size())
    {
        ClassInstance *c = classInstances[i];
        if (c->marked == 1)
        {
            markClass(c);
            classInstances[i] = classInstances.back();
            classInstances.pop();
            freeClass(c);
        }
        else
        {
            ++i;
        }
    }
}

void Interpreter::sweepMaps()
{

    size_t i = 0;
    while (i < mapInstances.size())
    {
        MapInstance *m = mapInstances[i];
        if (m->marked == 1)
        {
            markMap(m);
            mapInstances[i] = mapInstances.back();
            mapInstances.pop();
            freeMap(m);
        }
        else
        {
            ++i;
        }
    }
}

void Interpreter::sweepNativeClasses()
{

    size_t i = 0;
    while (i < nativeInstances.size())
    {
        NativeClassInstance *n = nativeInstances[i];
        if (n->marked == 1)
        {
            markNativeClass(n);
            nativeInstances[i] = nativeInstances.back();
            nativeInstances.pop();
            freeNativeClass(n);
        }
        else
        {
            ++i;
        }
    }
}

void Interpreter::sweepNativeStructs()
{

   // Info("Sweeping native structs (%zu)", nativeStructInstances.size());
    size_t i = 0;
    while (i < nativeStructInstances.size())
    {
        NativeStructInstance *n = nativeStructInstances[i];
        if (n->marked == 1)
        {
          //  Info("Sweeping native struct address: %p", (void*)n);
            markNativeStruct(n);    
            nativeStructInstances[i] = nativeStructInstances.back();
            nativeStructInstances.pop();
            freeNativeStruct(n);
        }
        else
        {
            ++i;
        }
    }

  //  Info("Sweeping native structs (%zu)", nativeStructInstances.size());
}

void Interpreter::sweepBuffers()
{

    size_t i = 0;
    while (i < bufferInstances.size())
    {
        BufferInstance *b = bufferInstances[i];
        if (b->marked == 1)
        {
            markBuffer(b);
            bufferInstances[i] = bufferInstances.back();
            bufferInstances.pop();
            freeBuffer(b);
        }
        else
        {
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
    else if (v.isBuffer())
        markBuffer(v.as.buffer);
    else if (v.isMap())
        markMap(v.as.map);
    else if (v.isNativeClass())
        markNativeClass(v.as.sClassInstance);
    else if (v.isNativeStructInstance())
        markNativeStruct(v.as.sNativeStruct);
}

void Interpreter::checkGC()
{
    if (!enbaledGC)
        return;

    if (totalAllocated > nextGC)
    {
        runGC();
    }
}

void Interpreter::runGC()
{
    gcInProgress = true;

    // if (totalAllocated<=nextGC)
    // {
    //     gcInProgress = false;
    //     return;
    // }

    size_t before = totalAllocated;

    sweepArrays();
    sweepMaps();
    sweepBuffers();
    sweepStructs();
    sweepClasses();
    sweepNativeClasses();
    sweepNativeStructs();

    nextGC = totalAllocated * 2;
    if (nextGC < 1024)
        nextGC = 1024; // Mínimo 1KB

    //Info("GC: %zu -> %zu bytes (next at %zu)", before, totalAllocated, nextGC);
    gcInProgress = false;
}
