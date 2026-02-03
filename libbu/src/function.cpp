#include "config.hpp"
#include "interpreter.hpp"
#include "pool.hpp"

Function::~Function()
{
   
    if (chunk)
    {
        chunk->clear();
        delete chunk;
    }
}

Function *Interpreter::addFunction(const char *name, int arity)
{
    String *pName = createString(name);
    Function *existing = nullptr;

    if (functionsMap.get(pName, &existing))
    {
        return nullptr;
    }

    Function *func = new Function();

    func->arity = arity;
    func->hasReturn = false;
    func->name = pName;
    func->chunk = new Code(16);
    func->index = functions.size();

    functionsMap.set(pName, func);
    functions.push(func);

    return func;
}
 

bool Interpreter::functionExists(const char *name)
{
    String *pName = createString(name);
    bool exists = functionsMap.exist(pName);
 
    return exists;
}



int Interpreter::registerNative(const char *name, NativeFunction func, int arity)
{
    String *nName = createString(name);
    NativeDef existing;
    if (nativesMap.get(nName, &existing))
    {

        return -1; // Já registrado
    }

    NativeDef def;
    def.name = nName;
    def.func = func;
    def.arity = arity;
    def.index = natives.size();

    nativesMap.set(nName, def);
    natives.push(def);

 //   Info("Registered native: %s (index=%d)", name, def.index);

    globals.set(nName, makeNative(def.index));

    // OPTIMIZATION: Also add to globalsArray for direct indexed access
    // The index in globalsArray will match the index assigned by the compiler
    // This requires the compiler to sync its globalIndices_ with native names
    uint16 globalIndex = globalsArray.size();
    globalsArray.push(makeNative(def.index));
    nativeGlobalIndices.set(nName, globalIndex);

    return def.index;
}

void Interpreter::destroyFunction(Function *func)
{
    if (!func)
        return;

    String *funcName = func->name;
    if (funcName)
    {
        Warning(" Remove Function %s", funcName->chars());

  
    }

    func->chunk->clear();

    delete func;

    // Libera memória
}