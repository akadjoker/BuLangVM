
#include "pch.h"
#include "Utils.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"
#include "Ast.hpp"



Value Interpreter::DEFAULT = to_none();

Interpreter::Interpreter()
{
    current = newCompiler("__main__", nullptr);
    generator = new ByteGenerator(this);
    global = Factory::as().CreateScope(nullptr);
    
}

Interpreter::~Interpreter()
{
    delete generator;

   
    Factory::as().Clear();
}

bool Interpreter::Load(const String &source)
{

    Parser parser(this);
    if (parser.Load(source))
    {
        return parser.Parse(generator);
    }

    return false;
}

void Interpreter::Disassemble()
{

    current->Disassemble();
   // compilers[0]->Disassemble();

    // for (u32 i = 0; i < compilers.size(); ++i)
    // {
    //     compilers[i]->Disassemble();
    // }
}

u8 Interpreter::Run()
{
    if (current == nullptr)
    {
        return 0;
    } 
    return current->Run();
}

void Interpreter::Clear()
{
}


Compiler *Interpreter::newCompiler(const Chars &name, Compiler *parent)
{
    Compiler *compiler = Factory::as().CreateCompiler(name, this, parent);
    compilers.push_back(compiler);
    compilerMap.insert( name, compiler );
    return compiler;
}

Compiler *Interpreter::addCompiler(const Chars &name, Compiler *parent)
{
    Compiler *compiler =  Factory::as().CreateCompiler(name, this, parent);
    //compilersGarbages.push_back(compiler);
    return compiler;
}

Compiler *Interpreter::getCompiler(const Chars &name)
{
    Compiler *compiler = nullptr;
    if(!compilerMap.find(name, compiler))
    {
        Error("Unknown compiler %s", name.c_str());
        return nullptr;
    }
    return compiler;
}


void Interpreter::Error(const String &message)
{
	Log(2, message.c_str());
    current->abort();
}



void Interpreter::Warning(const String& message)
{
	Log(1, message.c_str());
}

void Interpreter::Info(const String& message)
{
	Log(0, message.c_str());
}


void Interpreter::Error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(2, format, args);
    va_end(args);
    panicMode = true;
    current->abort();
    
}

void Interpreter::Warning(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(1, format, args);
    va_end(args);
}

void Interpreter::Info(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    Log(0, format, args);
    va_end(args);
}

bool Interpreter::push(Value v)
{
   return current->push(v);
}

Value Interpreter::pop()
{
    return current->pop();
}

Value Interpreter::peek(int offset)
{
    return current->peek(offset);
}

Value Interpreter::top()
{
    return current->top();
}

void Interpreter::pop(u32 count)
{
    current->pop(count);
}

bool Interpreter::push_int(int v)
{
    return push(to_integer(v));
}

bool Interpreter::push_bool(bool v)
{
    return push(to_boolean(v));
}

bool Interpreter::push_float(float v)
{
    return push(to_number(v));
}

bool Interpreter::push_double(double v)
{
    return push(to_number(v));
}

bool Interpreter::push_string(const char *v)
{
    return push(to_string_c(v));
}

bool Interpreter::push_string(const String &v)
{
    return push(to_string_c(v.c_str()));
}

bool Interpreter::is_bool(int index)
{
    return (current->stack[index].type == ValueType::VBOOLEAN);
}

bool Interpreter::is_number(int index)
{
    return (current->stack[index].type == ValueType::VNUMBER);
}

bool Interpreter::is_string(int index)
{
     return (current->stack[index].type == ValueType::VSTRING);
}

int Interpreter::as_int(int index)
{
     return as_integer(current->stack[index]);
}

float Interpreter::as_float(int index)
{
    return  static_cast<float>(as_number(current->stack[index]));
}

double Interpreter::as_double(int index)
{
    return as_number(current->stack[index]);
}

char *Interpreter::as_string(int index)
{
    const char* text = as_raw_string(current->stack[index]);
    return (char*) text; 
}
