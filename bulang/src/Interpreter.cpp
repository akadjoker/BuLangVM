
#include "pch.h"
#include "Utils.hpp"
#include "Interpreter.hpp"
#include "Parser.hpp"
#include "Lexer.hpp"
#include "Ast.hpp"


Interpreter::Interpreter()
{
    current = newCompiler("__main__", nullptr);
    generator = new ByteGenerator(this);
    
}

Interpreter::~Interpreter()
{
    delete generator;
    for (u32 i = 0; i < compilers.size(); ++i)
    {
        delete compilers[i];
    }
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

    for (u32 i = 0; i < compilers.size(); ++i)
    {
        compilers[i]->Disassemble();
    }
}

u8 Interpreter::Run()
{
    current->set_frame();
    return current->Run();
}

void Interpreter::Clear()
{
}

Compiler *Interpreter::newCompiler(const Chars &name, Compiler *parent)
{
    Compiler *compiler = new Compiler(name,this, parent);
    compilers.push_back(compiler);
    compilerMap.insert( name, compiler );
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