#include "interpreter.hpp"

void Interpreter::markRoots()
{

    if (globals.count)
    {
        globals.forEach([this](String *key, Value val)
                        { 
            if(val.isObject())
            {
                //printValueNl(val);
                markValue(val); 
            } });
    }

    for (size_t i = 0; i < aliveProcesses.size(); i++)
    {
        Process *proc = aliveProcesses[i];

        // Marca privates
        for (int j = 0; j < MAX_PRIVATES; j++)
        {
            markValue(proc->privates[j]);
        }

        // Marca todas as fibers
        if (proc->fibers)
        {
            for (int f = 0; f < proc->totalFibers; f++)
            {
                Fiber *fiber = &proc->fibers[f];
                if (fiber->state != FiberState::DEAD)
                {
                    for (Value *v = fiber->stack; v < fiber->stackTop; v++)
                    {
                        markValue(*v);
                    }
                }
            }
        }
    }
}

void Interpreter::markObject(GCObject *obj)
{
    if (!obj || obj->marked)
        return;

    obj->marked = 1;

    // Info("Marking %d", (int) obj->type);

    switch (obj->type)
    {
    case GCObjectType::STRUCT:
    {
        StructInstance *s = static_cast<StructInstance *>(obj);
        for (size_t i = 0; i < s->values.size(); i++)
        {
            markValue(s->values[i]);
        }
        break;
    }

    case GCObjectType::CLASS:
    {
        ClassInstance *c = static_cast<ClassInstance *>(obj);
        for (size_t i = 0; i < c->fields.size(); i++)
        {
            markValue(c->fields[i]);
        }
        break;
    }

    case GCObjectType::ARRAY:
    {
        ArrayInstance *a = static_cast<ArrayInstance *>(obj);
        for (size_t i = 0; i < a->values.size(); i++)
        {
            markValue(a->values[i]);
        }
        break;
    }

    case GCObjectType::MAP:
    {
        MapInstance *m = static_cast<MapInstance *>(obj);
        m->table.forEach([this](String *key, Value val)
                         { markValue(val); });
        break;
    }

    case GCObjectType::BUFFER:
    case GCObjectType::NATIVE_CLASS:
    case GCObjectType::NATIVE_STRUCT:
        //  Sem conteúdo para marcar ???
        break;
    }
}

void Interpreter::markValue(const Value &v)
{
    if (v.isStructInstance())
    {
        // printValueNl(v);
        markObject(v.as.sInstance);
    }
    else if (v.isClassInstance())
    {
        markObject(v.as.sClass);
    }
    else if (v.isArray())
    {
        // printValueNl(v);
        markObject(v.as.array);
    }
    else if (v.isMap())
    {
        markObject(v.as.map);
    }
    else if (v.isBuffer())
    {
        markObject(v.as.buffer);
    }
    else if (v.isNativeClassInstance())
    {
        markObject(v.as.sClassInstance);
    }
    else if (v.isNativeStructInstance())
    {
        markObject(v.as.sNativeStruct);
    }
}

void Interpreter::sweep()
{
    //  Info("GC Sweep start");

    GCObject **obj = &gcObjects;
    size_t freed = 0;

    while (*obj)
    {
        if ((*obj)->marked == 0)
        {
            GCObject *unreached = *obj;
            *obj = unreached->next;
            freeObject(unreached);
            freed++;
        }
        else
        {
            //  Desmarca para próximo ciclo
            (*obj)->marked = 0;
            obj = &(*obj)->next;
        }
    }

    //   Info("GC Sweep freed %zu objects", freed);
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
    }
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
    if (gcInProgress)
        return;
    gcInProgress = true;

    size_t bytesBefore = totalAllocated;
    size_t objectsBefore = totalArrays + totalClasses + totalStructs + totalMaps + totalBuffers + totalNativeClasses + totalNativeStructs;

    markRoots();
    sweep();

    size_t objectCount = totalArrays + totalClasses + totalStructs + totalMaps + totalBuffers + totalNativeClasses + totalNativeStructs;

    size_t bytesFreed = bytesBefore - totalAllocated;
    size_t objectsFreed = objectsBefore - objectCount;

    nextGC = static_cast<size_t>(totalAllocated * GC_GROWTH_FACTOR);
    if (nextGC < MIN_GC_THRESHOLD)
    {
        nextGC = MIN_GC_THRESHOLD;
    }
    if (nextGC > MAX_GC_THRESHOLD)
    {
        nextGC = MAX_GC_THRESHOLD;
    }

// Info("GC: End - Freed %zu objects (%.2f KB). Remaining: %zu objects (%.2f KB). Next GC: %.2f KB",
//          objectsFreed, bytesFreed / 1024.0,
//          objectCount, totalAllocated / 1024.0,
//          nextGC / 1024.0);

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
    return count;
}

void Interpreter::clearAllGCObjects()
{
    Info("Arena cleared");

    if (!gcObjects)
        return;

    GCObject **obj = &gcObjects;
    size_t freed = 0;

    while (*obj)
    {

        GCObject *toFree = *obj;
        *obj = toFree->next;

        freeObject(toFree);
        freed++;
    }

    Info("Arena cleared (%zu objects freed)", freed);
}