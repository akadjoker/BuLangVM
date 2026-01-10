#include <iostream>
#include <string>
#include <cmath>
#include <emscripten.h>
#include <emscripten/bind.h>
#include "platform.hpp"
#include "interpreter.hpp"
#include "Outputcapture.h"
#include "webapi.h"
#include "random.hpp"

OutputCapture *g_currentOutput = nullptr;

 
 
Value native_rand(Interpreter *vm, int argCount, Value *args)
{

    if (argCount == 0)
    {
        return vm->makeDouble(RandomGenerator::instance().randFloat());
    }
    else if (argCount == 1)
    {
        double value = args[0].isInt() ? (double)args[0].asInt() : args[0].asDouble();
        return vm->makeDouble(RandomGenerator::instance().randFloat(0, value));
    }
    else
    {
        double min = args[0].isInt() ? (double)args[0].asInt() : args[0].asDouble();
        double max = args[1].isInt() ? (double)args[1].asInt() : args[1].asDouble();
        return vm->makeDouble(RandomGenerator::instance().randFloat(min, max));
    }
    return vm->makeNil();
}


static void valueToString(const Value &v, std::string &out)
{
    char buffer[256];

    switch (v.type)
    {
    case ValueType::NIL:
        out += "nil";
        break;
    case ValueType::BOOL:
        out += v.as.boolean ? "true" : "false";
        break;
    case ValueType::BYTE:
        snprintf(buffer, 256, "%u", v.as.byte);
        out += buffer;
        break;
    case ValueType::INT:
        snprintf(buffer, 256, "%d", v.as.integer);
        out += buffer;
        break;
    case ValueType::UINT:
        snprintf(buffer, 256, "%u", v.as.unsignedInteger);
        out += buffer;
        break;
    case ValueType::FLOAT:
        snprintf(buffer, 256, "%.2f", v.as.real);
        out += buffer;
        break;
    case ValueType::DOUBLE:
        snprintf(buffer, 256, "%.2f", v.as.number);
        out += buffer;
        break;
    case ValueType::STRING:
        out += v.as.string->chars();
        break;
    case ValueType::ARRAY:
        out += "[array]";
        break;
    case ValueType::MAP:
        out += "{map}";
        break;
    default:
        out += "<object>";
    }
}



Value native_ticks(Interpreter *vm, int argCount, Value *args)
{
    if (argCount != 1 || args[0].type != ValueType::DOUBLE)
    {
        vm->runtimeError("ticks expects double as argument");
        return vm->makeNil();
    }
    vm->update(args[0].asDouble());
    return vm->makeNil();
}


Value native_format(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("format expects string as first argument");
        return vm->makeNil();
    }

  

    const char *fmt = args[0].as.string->chars();
    std::string result;
    int argIndex = 1;

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] == '{' && fmt[i + 1] == '}')
        {
            if (argIndex < argCount)
            {
                valueToString(args[argIndex++], result);
            }
            i++;
        }
        else
        {
            result += fmt[i];
        }
    }

    return vm->makeString(result.c_str());
}

Value native_write(Interpreter *vm, int argCount, Value *args)
{
    if (argCount < 1 || args[0].type != ValueType::STRING)
    {
        vm->runtimeError("write expects string as first argument");
        return vm->makeNil();
    }

    const char *fmt = args[0].as.string->chars();
    std::string result;
    int argIndex = 1;

    for (int i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] == '{' && fmt[i + 1] == '}')
        {
            if (argIndex < argCount)
            {
                valueToString(args[argIndex++], result);
            }
            i++;
        }
        else
        {
            result += fmt[i];
        }
    }

    OsPrintf("%s", result.c_str());
    return vm->makeNil();
}

 
// ============================================
// Execute Code (exposto ao JS)
// ============================================

std::string executeCode(const std::string &code)
{
    OutputCapture output;
    g_currentOutput = &output; // Ativa captura - OsPrintf vai usar isto!

    Interpreter vm;
 
    vm.registerNative("write", native_write, -1);
    vm.registerNative("format", native_format, -1);
    vm.registerNative("ticks", native_ticks, 1);
    vm.registerNative("rand", native_rand, 1);
    
    registerWebNatives(&vm);

    std::string result;

    try
    {

        if (!vm.run(code.c_str(), false))
        {

            std::string compileError = output.getOutput();
            if (compileError.empty())
            {
                result = "❌ Compilation Error";
            }
            else
            {
                result = compileError;
            }
        }
        else
        {
            // Sucesso
            result = output.getOutput();
            if (result.empty())
            {
                result = "✓ Executed successfully (no output)";
            }
        }
    }
    catch (const std::exception &e)
    {

        result = std::string("❌ Runtime Error: ") + e.what();
        std::string vmOutput = output.getOutput();
        if (!vmOutput.empty())
        {
            result += ": \n" + vmOutput;
        }
    }

    g_currentOutput = nullptr;
    return result;
}

// ============================================
// Embind Bindings
// ============================================

EMSCRIPTEN_BINDINGS(bulang_playground)
{
    emscripten::function("executeCode", &executeCode);
}
