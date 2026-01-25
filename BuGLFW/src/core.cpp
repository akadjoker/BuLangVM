#include "bindings.hpp"
 

namespace GLFWBindings
{
    // =============================================================
    // INITIALIZATION & TERMINATION
    // =============================================================

    int native_glfwInit(Interpreter *vm, int argc, Value *args)
    {
        int result = glfwInit();
        vm->pushBool(result == GLFW_TRUE);
        return 1;
    }

    int native_glfwTerminate(Interpreter *vm, int argc, Value *args)
    {
        glfwTerminate();
        return 0;
    }

    int native_glfwInitHint(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwInitHint expects 2 arguments (hint, value)");
            return 0;
        }

        int hint = args[0].asNumber();
        int value = args[1].asNumber();
        glfwInitHint(hint, value);
        return 0;
    }

    // =============================================================
    // VERSION
    // =============================================================

    int native_glfwGetVersion(Interpreter *vm, int argc, Value *args)
    {
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);

        vm->pushInt(major);
        vm->pushInt(minor);
        vm->pushInt(revision);
        return 3;
      
    }

    int native_glfwGetVersionString(Interpreter *vm, int argc, Value *args)
    {
        const char *version = glfwGetVersionString();
        vm->pushString(version);
        return 1;
    }

    // =============================================================
    // ERROR HANDLING
    // =============================================================

    int native_glfwGetError(Interpreter *vm, int argc, Value *args)
    {
        const char *description;
        int error = glfwGetError(&description);

        vm->pushInt(error);
        vm->pushString(description);
        return 2;
    }

    // =============================================================
    // TIME
    // =============================================================

    int native_glfwGetTime(Interpreter *vm, int argc, Value *args)
    {
        double time = glfwGetTime();
         vm->pushDouble(time);
        return 1;
    }

    int native_glfwSetTime(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwSetTime expects 1 argument");
            return 0;
        }

        double time = args[0].asNumber();
        glfwSetTime(time);
        return 0;
    }

    int native_glfwGetTimerValue(Interpreter *vm, int argc, Value *args)
    {
        uint64_t value = glfwGetTimerValue();
         vm->pushDouble((double)value);
         return 1;
    }

    int native_glfwGetTimerFrequency(Interpreter *vm, int argc, Value *args)
    {
        uint64_t freq = glfwGetTimerFrequency();
         vm->pushDouble((double)freq);
         return 1;
    }

    // =============================================================
    // CLIPBOARD
    // =============================================================

    int native_glfwSetClipboardString(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwSetClipboardString expects 2 arguments (window, string)");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetClipboardString expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        const char *text = args[1].asStringChars();
        glfwSetClipboardString(window, text);
        return 0;
    }

    int native_glfwGetClipboardString(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetClipboardString expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetClipboardString expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        const char *text = glfwGetClipboardString(window);
        vm->pushString(text);
        return 1;
    }

    // =============================================================
    // VULKAN SUPPORT
    // =============================================================

    int native_glfwVulkanSupported(Interpreter *vm, int argc, Value *args)
    {
        int result = glfwVulkanSupported();
         vm->pushBool(result == GLFW_TRUE);
        return 1;
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
            .addByte("GLFW_NO_ERROR", GLFW_NO_ERROR)
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
            .addByte("GLFW_TRUE", GLFW_TRUE)
            .addByte("GLFW_FALSE", GLFW_FALSE);
    }
}