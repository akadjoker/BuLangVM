#include "bindings.hpp"


namespace GLFWBindings
{
    // =============================================================
    // CONTEXT FUNCTIONS
    // =============================================================

    int native_glfwMakeContextCurrent(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwMakeContextCurrent expects 1 argument");
            return 0;
        }

        GLFWwindow *window = nullptr;
        if (args[0].isPointer())
            window = (GLFWwindow *)args[0].asPointer();

        glfwMakeContextCurrent(window);
        return 0;
    }

    int native_glfwGetCurrentContext(Interpreter *vm, int argc, Value *args)
    {
        GLFWwindow *window = glfwGetCurrentContext();
        vm->push(vm->makePointer(window));
        return 1;

    }

    int native_glfwSwapBuffers(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwSwapBuffers expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSwapBuffers expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwSwapBuffers(window);
        return 0;
    }

    int native_glfwSwapInterval(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwSwapInterval expects 1 argument");
            return 0;
        }

        int interval = args[0].asNumber();
        glfwSwapInterval(interval);
        return 0;
    }

    int native_glfwExtensionSupported(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwExtensionSupported expects 1 argument");
            return 0;
        }
        if (!args[0].isString())
        {
            Error("glfwExtensionSupported expects string");
            return 0;
        }

        const char *extension = args[0].asStringChars();
        int result = glfwExtensionSupported(extension);
         vm->pushBool(result == GLFW_TRUE);
        return 1;
    }

    void register_context(ModuleBuilder &mod)
    {
        mod
            // =============================================================
            // CONTEXT FUNCTIONS
            // =============================================================
            .addFunction("glfwMakeContextCurrent", native_glfwMakeContextCurrent, 1)
            .addFunction("glfwGetCurrentContext", native_glfwGetCurrentContext, 0)
            .addFunction("glfwSwapBuffers", native_glfwSwapBuffers, 1)
            .addFunction("glfwSwapInterval", native_glfwSwapInterval, 1)
            .addFunction("glfwExtensionSupported", native_glfwExtensionSupported, 1)

            // =============================================================
            // CONTEXT CREATION API HINTS
            // =============================================================
            .addInt("GLFW_CLIENT_API", GLFW_CLIENT_API)
            .addInt("GLFW_CONTEXT_VERSION_MAJOR", GLFW_CONTEXT_VERSION_MAJOR)
            .addInt("GLFW_CONTEXT_VERSION_MINOR", GLFW_CONTEXT_VERSION_MINOR)
            .addInt("GLFW_CONTEXT_REVISION", GLFW_CONTEXT_REVISION)
            .addInt("GLFW_CONTEXT_ROBUSTNESS", GLFW_CONTEXT_ROBUSTNESS)
            .addInt("GLFW_OPENGL_FORWARD_COMPAT", GLFW_OPENGL_FORWARD_COMPAT)
            .addInt("GLFW_OPENGL_DEBUG_CONTEXT", GLFW_OPENGL_DEBUG_CONTEXT)
            .addInt("GLFW_OPENGL_PROFILE", GLFW_OPENGL_PROFILE)
            .addInt("GLFW_CONTEXT_RELEASE_BEHAVIOR", GLFW_CONTEXT_RELEASE_BEHAVIOR)
            .addInt("GLFW_CONTEXT_NO_ERROR", GLFW_CONTEXT_NO_ERROR)
            .addInt("GLFW_CONTEXT_CREATION_API", GLFW_CONTEXT_CREATION_API)

            // =============================================================
            // CLIENT API VALUES
            // =============================================================
            .addInt("GLFW_NO_API", GLFW_NO_API)
            .addInt("GLFW_OPENGL_API", GLFW_OPENGL_API)
            .addInt("GLFW_OPENGL_ES_API", GLFW_OPENGL_ES_API)

            // =============================================================
            // CONTEXT ROBUSTNESS VALUES
            // =============================================================
            .addInt("GLFW_NO_ROBUSTNESS", GLFW_NO_ROBUSTNESS)
            .addInt("GLFW_NO_RESET_NOTIFICATION", GLFW_NO_RESET_NOTIFICATION)
            .addInt("GLFW_LOSE_CONTEXT_ON_RESET", GLFW_LOSE_CONTEXT_ON_RESET)

            // =============================================================
            // OPENGL PROFILE VALUES
            // =============================================================
            .addInt("GLFW_OPENGL_ANY_PROFILE", GLFW_OPENGL_ANY_PROFILE)
            .addInt("GLFW_OPENGL_CORE_PROFILE", GLFW_OPENGL_CORE_PROFILE)
            .addInt("GLFW_OPENGL_COMPAT_PROFILE", GLFW_OPENGL_COMPAT_PROFILE)

            // =============================================================
            // CONTEXT RELEASE BEHAVIOR VALUES
            // =============================================================
            .addInt("GLFW_ANY_RELEASE_BEHAVIOR", GLFW_ANY_RELEASE_BEHAVIOR)
            .addInt("GLFW_RELEASE_BEHAVIOR_FLUSH", GLFW_RELEASE_BEHAVIOR_FLUSH)
            .addInt("GLFW_RELEASE_BEHAVIOR_NONE", GLFW_RELEASE_BEHAVIOR_NONE)

            // =============================================================
            // CONTEXT CREATION API VALUES
            // =============================================================
            .addInt("GLFW_NATIVE_CONTEXT_API", GLFW_NATIVE_CONTEXT_API)
            .addInt("GLFW_EGL_CONTEXT_API", GLFW_EGL_CONTEXT_API)
            .addInt("GLFW_OSMESA_CONTEXT_API", GLFW_OSMESA_CONTEXT_API);
    }
}