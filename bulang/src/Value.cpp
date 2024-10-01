#include "pch.h"
#include "Value.hpp"
#include "Utils.hpp"
#include "Arena.hpp"
#include "Interpreter.hpp"

void *Malloc(u32 size, const char *file, u32 line);
void Free(void *p, u32 size);

#define ARENA_ALLOC(size) Malloc(size, __FILE__, __LINE__)

static float memoryInMB(size_t bytes)
{
    return static_cast<float>(bytes) / (1024.0f * 1024.0f);
}

static float memoryInKB(size_t bytes)
{
    return static_cast<float>(bytes) / 1024.0f;
}

struct AllocationInfo
{
    u32 size;
    Chars file;
    u32 line;
};

static Map<void *, AllocationInfo> g_allocations;
static bool g_debugMode = false;

void *Malloc(u32 size, const char *file, u32 line)
{
    void *mem = Factory::as().arena.Allocate(size);
    if (g_debugMode && mem != nullptr)
    {
        g_allocations.insert(mem, {size, file, line});
    }
    return mem;
}

void Free(void *mem, u32 size)
{
    if (g_debugMode && mem != nullptr)
    {

        g_allocations.erase(mem);
    }

    Factory::as().arena.Free(mem, size);
}
static void ReportMemoryLeaks()
{
    if (!g_debugMode)
        return;

    if (!g_allocations.size())
    {
        WARNING("Memory Leaks Detected:");
        // for (const auto &entry : g_allocations)
        // {
        // 	WARNING("Leaked %d bytes at %p, allocated at %s:%d",
        // 		   entry.second.size,
        // 		   entry.first,
        // 		   entry.second.file.c_str(),
        // 		   entry.second.line);
        // }
    }
}

static const char *memoryIn(size_t bytes)
{
    if (bytes >= 1.0e6)
    {
        return FormatText("%.2f MB", memoryInMB(bytes));
    }
    else if (bytes >= 1.0e3)
    {
        return FormatText("%.2f KB", memoryInKB(bytes));
    }
    return FormatText("%zu bytes", bytes);
}

extern void debugValue(const Value& value);
extern void printValueln(const Value &v);

Traceable::Traceable()
{
    marked = false;
    id = 0;
    type = ObjectType::UNDEFINED;
}

Traceable::~Traceable()
{
}

StringObject::StringObject(const Chars &str)
{
    string = str;
    type = ObjectType::OSTRING;
}

void Factory::Clear() 
{
    INFO("Arena used %s of memory", memoryIn(arena.size()));

    INFO("Strings %d", strings.size());
    for (u32 i = 0; i < strings.size(); i++)
    {
        StringObject *str = strings[i];
        str->~StringObject();
        ARENA_FREE(str, sizeof(StringObject));
    }

    INFO("Scopes %d", scopes.size());
    for (u32 i = 0; i < scopes.size(); i++)
    {
        Scope *scope = scopes[i];
        scope->~Scope();
        ARENA_FREE(scope, sizeof(Scope));
    }

    INFO("Compilers %d", compilers.size());
    for (u32 i = 0; i < compilers.size(); i++)
    {
        Compiler *compiler = compilers[i];
        compiler->~Compiler();
        ARENA_FREE(compiler, sizeof(Compiler));
    }

    arena.Clear();

    ReportMemoryLeaks();

     
}

Factory &Factory::as()
{
    static Factory factory;
    return factory;
}

Factory::Factory()
{
     scopes.reserve(1024);
     strings.reserve(1024);
}

Factory::~Factory()
{
}

Scope *Factory::getScope()
{

    if (pool.empty())
    {
        for (int i = 0; i < 1024; i++)
        {
            pool.emplace_back(nullptr);
        }
    }

    Scope *scope = &pool.back();
    pool.pop_back();
    return scope;
}

StringObject *Factory::CreateStringObject(const Chars &value)
{
    void *p = ARENA_ALLOC(sizeof(StringObject));
    StringObject *obj = new (p) StringObject(value);
    strings.push_back(obj);
    return obj;
}

Scope *Factory::CreateScope(Scope *parent)
{

    void *p = ARENA_ALLOC(sizeof(Scope));
    Scope *obj = new (p) Scope(parent);
    scopes.push_back(obj);
    return obj;
}

void Factory::DestroyScope(Scope *scope)
{
  //  scope->~Scope();
  //  ARENA_FREE(scope, sizeof(Scope));
}

Compiler *Factory::CreateCompiler(const Chars &name, Interpreter *i, Compiler *parent)
{
    void *p = ARENA_ALLOC(sizeof(Compiler));
    Compiler *obj = new (p) Compiler(name, i, parent);
    compilers.push_back(obj);
    return obj;
   
}

void Factory::DestroyCompiler(Compiler *compiler)
{

    compiler->~Compiler();
    ARENA_FREE(compiler, sizeof(Compiler));
}

//********************************************************** */

static u32 g_scopeDepth = 0;

Scope::Scope() : parent(nullptr)
{
    type = ObjectType::OSCOPE;
    depth = g_scopeDepth++;
}

Scope::Scope(Scope *parent) : parent(parent)
{
    type = ObjectType::OSCOPE;
    depth = g_scopeDepth++;
}

Scope::~Scope()
{
    g_scopeDepth--;
    //    values.clear();
}

void Scope::setParent(Scope *parent)
{
    this->parent = parent;
}

bool Scope::define(const Chars &name, Value value)
{
    return values.insert(name, value);
}

bool Scope::assign(const Chars &name, Value value)
{
    Value old = to_none();
    if (values.replace(name, value, old))
    {
        return true;
    }
    if (parent != nullptr)
        return parent->assign(name, value);
    return false;
}

bool Scope::lookup(const Chars &name, Value &value)
{
    if (values.find(name, value))
        return true;

    if (parent != nullptr)
        return parent->lookup(name, value);
    return false;
}



void Scope::print(bool withParent)
{

    if (parent != nullptr && withParent)
    {
        INFO("Scope %d", depth);
        parent->print();
    }
    
    const u32 size = values.size();
    const u32 count = values.count();
    INFO("Scope %d count %d size %d", depth, count ,size);
    
    for (u32 i = 0; i < size; i++)
    {
        KeyItem<Chars, Value> *root = values.root(i);
        while (root)
        {
            printf("%s = ", root->key.c_str());
            printValueln(root->value);
            root = root->next;
        }
    }
    // for (auto it = values.begin(); it != values.end(); it++)
    // {
    //     const String name = it->first;
    //     const Value value = it->second;
    //     printf("%s = ", name.c_str());
    //     debugValue(value);
    //     printf("\n");
    // }
}

Value to_integer(int value)
{
    Value v;
    v.type = ValueType::VNUMBER;
    v.number = static_cast<double>(value);
    return v;
}

Value to_number(double value)
{
    Value v;
    v.type = ValueType::VNUMBER;
    v.number = value;
    return v;
}

Value to_boolean(bool value)
{
    Value v;
    v.type = ValueType::VBOOLEAN;
    v.boolean = value;
    return v;
}

Value to_string(const Chars &value)
{
    Value v;
    v.type = ValueType::VSTRING;
    v.string = Factory::as().CreateStringObject(value);
    return v;
}

Value to_string_c(const char *value)
{
    Value v;
    v.type = ValueType::VSTRING;
    v.string = Factory::as().CreateStringObject(value);
    return v;
}

Value to_none()
{
    Value v;
    v.type = ValueType::VNONE;
    return v;
}

bool is_number(Value value)
{
    return value.type == ValueType::VNUMBER;
}

bool is_string(Value value)
{
    return value.type == ValueType::VSTRING;
}

bool is_boolean(Value value)
{
    return value.type == ValueType::VBOOLEAN;
}

bool is_none(Value value)
{
    return value.type == ValueType::VNONE;
}

int as_integer(Value value)
{
    return static_cast<int>(value.number);
}

double as_number(Value value)
{
    return value.number;
}

bool as_boolean(Value value)
{
    return value.boolean;
}

Chars as_string(Value value)
{
    if (value.type != ValueType::VSTRING)
    {
        return Chars("nil");
    }
    return value.string->string;
}

const char *as_raw_string(Value value)
{
    if (value.type != ValueType::VSTRING)
    {
        return "nil";
    }
    return value.string->string.c_str();
}

Value as_clone(Value value)
{
    if (value.type == ValueType::VSTRING)
    {
        return to_string(value.string->string);
    }
    else if (value.type == ValueType::VNUMBER)
    {
        return to_number(value.number);
    }
    else if (value.type == ValueType::VBOOLEAN)
    {
        return to_boolean(value.boolean);
    }
    return to_none();
}

bool is_falsey(Value value)
{
    if (value.type == ValueType::VNONE)
    {
        return true;
    }
    else if (value.type == ValueType::VNUMBER)
    {
        int i = static_cast<int>(value.number);
        return (i == 0);
    }
    else if (value.type == ValueType::VSTRING)
    {
        return value.string->string.empty();
    }
    else if (value.type == ValueType::VBOOLEAN)
    {
        return !value.boolean;
    }
    return true;
}

bool is_match(Value a, Value b)
{
    if (a.type != b.type)
    {
        return false;
    }
    if (a.type == ValueType::VSTRING)
    {
        return a.string->string == b.string->string;
    }
    else if (a.type == ValueType::VBOOLEAN)
    {
        return a.boolean == b.boolean;
    } else if (a.type == ValueType::VNUMBER) 
    {
        return a.number == b.number;
    }
    return false;
   
}

