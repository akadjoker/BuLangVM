#pragma once

#include "Config.hpp"
#include "String.hpp"
#include "Chars.hpp"
#include "Map.hpp"
#include "Mem.hpp"
#include "Arena.hpp"

void *Malloc(u32 size, const char *file, u32 line);
void Free(void *p, u32 size);

#define ARENA_ALLOC(size) Malloc(size, __FILE__, __LINE__)
#define ARENA_FREE(mem, size) Free(mem, size)

enum class ObjectType
{
    UNDEFINED = 0,
    OSTRING,
    OBOOLEAN,
    OLIST,
    OMAP,
    OFUNCTION,
    OPROCESS,
    OSCOPE,
};

enum class ValueType
{
    VUNDEFINED = 0,
    VNUMBER,
    VSTRING,
    VBOOLEAN,
    VNATIVE,
    VUSER,
    VPROCESS,
    VNONE,

};

class Factory;
struct Scope;
struct Traceable;
struct StringObject;

struct Value
{
    u8 flags;
    ValueType type;
    union
    {
        double number;
        StringObject *string;
        bool boolean;
    };
};

struct Traceable
{
    u64 id;
    bool marked;
    ObjectType type;
    

    Traceable();

    virtual ~Traceable();
};

struct StringObject : public Traceable
{
    Chars string;
    StringObject(const Chars &str);
};

struct Function : public Traceable
{
    Chars name;
    SharedPtr<Scope> scope;
    Function();
};

struct Scope : public Traceable
{
    Scope();
    Scope(Scope *parent);
    ~Scope();

    void setParent(Scope *parent);

    bool define(const Chars &name, Value value);
    bool assign(const Chars &name, Value value);
    bool lookup(const Chars &name, Value &value);


    Scope(const Scope& other);                   // Copy constructor
    Scope& operator=(const Scope& other);        // Copy assignment operator
    Scope(Scope&& other) noexcept;               // Move constructor
    Scope& operator=(Scope&& other) noexcept;    // Move assignment operator


    void print(bool parent = false);

    Map<Chars, Value> values;
    Scope *parent = nullptr;
    u32 depth = 0;
};

class Factory
{
public:
    void Clear() const;

    static Factory &as();

    StringObject *CreateStringObject(const Chars &value);

    Scope *CreateScope(Scope *parent = nullptr);

    void DestroyScope(Scope *scope);

    BlockArena arena;

private:
    Factory();
    ~Factory();

    Scope *getScope();

    Vector<StringObject *> strings;
    Vector<Scope *> scopes;
    Vector<Scope> pool;
};




Value to_integer(int value);
Value to_number(double value);
Value to_boolean(bool value);
Value to_string(const Chars &value);
Value to_string_c(const char *value);
Value to_none();

bool is_number(Value value);
bool is_string(Value value);
bool is_boolean(Value value);
bool is_none(Value value);


int         as_integer(Value value);
double      as_number(Value value);
bool        as_boolean(Value value);
Chars       as_string(Value value);
const char* as_raw_string(Value value);

Value as_clone(Value value);
bool is_falsey(Value value);
bool is_match(Value a, Value b);