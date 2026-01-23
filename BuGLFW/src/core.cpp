#include "bindings.hpp"
 

namespace GLFWBindings
{
    // =============================================================
    // INITIALIZATION & TERMINATION
    // =============================================================

    Value native_glfwInit(Interpreter *vm, int argc, Value *args)
    {
        int result = glfwInit();
        return vm->makeInt(result);
    }

    Value native_glfwTerminate(Interpreter *vm, int argc, Value *args)
    {
        glfwTerminate();
        return vm->makeNil();
    }

    Value native_glfwInitHint(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwInitHint expects 2 arguments (hint, value)");
            return vm->makeNil();
        }

        int hint = args[0].asNumber();
        int value = args[1].asNumber();
        glfwInitHint(hint, value);
        return vm->makeNil();
    }

    // =============================================================
    // VERSION
    // =============================================================

    Value native_glfwGetVersion(Interpreter *vm, int argc, Value *args)
    {
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("major").asString(), vm->makeInt(major));
        map->table.set(vm->makeString("minor").asString(), vm->makeInt(minor));
        map->table.set(vm->makeString("revision").asString(), vm->makeInt(revision));
        return result;
    }

    Value native_glfwGetVersionString(Interpreter *vm, int argc, Value *args)
    {
        const char *version = glfwGetVersionString();
        return vm->makeString(version);
    }

    // =============================================================
    // ERROR HANDLING
    // =============================================================

    Value native_glfwGetError(Interpreter *vm, int argc, Value *args)
    {
        const char *description;
        int error = glfwGetError(&description);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("code").asString(), vm->makeInt(error));
        map->table.set(vm->makeString("description").asString(), 
                      description ? vm->makeString(description) : vm->makeNil());
        return result;
    }

    // =============================================================
    // TIME
    // =============================================================

    Value native_glfwGetTime(Interpreter *vm, int argc, Value *args)
    {
        double time = glfwGetTime();
        return vm->makeDouble(time);
    }

    Value native_glfwSetTime(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwSetTime expects 1 argument");
            return vm->makeNil();
        }

        double time = args[0].asNumber();
        glfwSetTime(time);
        return vm->makeNil();
    }

    Value native_glfwGetTimerValue(Interpreter *vm, int argc, Value *args)
    {
        uint64_t value = glfwGetTimerValue();
        return vm->makeDouble((double)value);
    }

    Value native_glfwGetTimerFrequency(Interpreter *vm, int argc, Value *args)
    {
        uint64_t freq = glfwGetTimerFrequency();
        return vm->makeDouble((double)freq);
    }

    // =============================================================
    // CLIPBOARD
    // =============================================================

    Value native_glfwSetClipboardString(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwSetClipboardString expects 2 arguments (window, string)");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetClipboardString expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        const char *text = args[1].asStringChars();
        glfwSetClipboardString(window, text);
        return vm->makeNil();
    }

    Value native_glfwGetClipboardString(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetClipboardString expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetClipboardString expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        const char *text = glfwGetClipboardString(window);
        return text ? vm->makeString(text) : vm->makeString("");
    }

    // =============================================================
    // VULKAN SUPPORT
    // =============================================================

    Value native_glfwVulkanSupported(Interpreter *vm, int argc, Value *args)
    {
        int result = glfwVulkanSupported();
        return vm->makeBool(result == GLFW_TRUE);
    }

    void register_core(ModuleBuilder &mod)
    {
        mod
            // =============================================================
            // INITIALIZATION & TERMINATION
            // =============================================================
            .addFunction("glfwInit", native_glfwInit, 0)
            .addFunction("glfwTerminate", native_glfwTerminate, 0)
            .addFunction("glfwInitHint", native_glfwInitHint, 2)

            // =============================================================
            // VERSION
            // =============================================================
            .addFunction("glfwGetVersion", native_glfwGetVersion, 0)
            .addFunction("glfwGetVersionString", native_glfwGetVersionString, 0)

            // =============================================================
            // ERROR HANDLING
            // =============================================================
            .addFunction("glfwGetError", native_glfwGetError, 0)

            // =============================================================
            // TIME
            // =============================================================
            .addFunction("glfwGetTime", native_glfwGetTime, 0)
            .addFunction("glfwSetTime", native_glfwSetTime, 1)
            .addFunction("glfwGetTimerValue", native_glfwGetTimerValue, 0)
            .addFunction("glfwGetTimerFrequency", native_glfwGetTimerFrequency, 0)

            // =============================================================
            // CLIPBOARD
            // =============================================================
            .addFunction("glfwSetClipboardString", native_glfwSetClipboardString, 2)
            .addFunction("glfwGetClipboardString", native_glfwGetClipboardString, 1)

            // =============================================================
            // VULKAN SUPPORT
            // =============================================================
            .addFunction("glfwVulkanSupported", native_glfwVulkanSupported, 0)

            // =============================================================
            // ERROR CODES
            // =============================================================
            .addInt("GLFW_NO_ERROR", GLFW_NO_ERROR)
            .addInt("GLFW_NOT_INITIALIZED", GLFW_NOT_INITIALIZED)
            .addInt("GLFW_NO_CURRENT_CONTEXT", GLFW_NO_CURRENT_CONTEXT)
            .addInt("GLFW_INVALID_ENUM", GLFW_INVALID_ENUM)
            .addInt("GLFW_INVALID_VALUE", GLFW_INVALID_VALUE)
            .addInt("GLFW_OUT_OF_MEMORY", GLFW_OUT_OF_MEMORY)
            .addInt("GLFW_API_UNAVAILABLE", GLFW_API_UNAVAILABLE)
            .addInt("GLFW_VERSION_UNAVAILABLE", GLFW_VERSION_UNAVAILABLE)
            .addInt("GLFW_PLATFORM_ERROR", GLFW_PLATFORM_ERROR)
            .addInt("GLFW_FORMAT_UNAVAILABLE", GLFW_FORMAT_UNAVAILABLE)

            // =============================================================
            // BOOLEAN VALUES
            // =============================================================
            .addInt("GLFW_TRUE", GLFW_TRUE)
            .addInt("GLFW_FALSE", GLFW_FALSE);
    }
}