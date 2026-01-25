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
