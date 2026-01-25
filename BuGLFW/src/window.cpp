#include "bindings.hpp"

namespace GLFWBindings
{
    // =============================================================
    // WINDOW HINTS
    // =============================================================

    int native_glfwDefaultWindowHints(Interpreter *vm, int argc, Value *args)
    {
        glfwDefaultWindowHints();
        return 0;
    }

    int native_glfwWindowHint(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwWindowHint expects 2 arguments (hint, value)");
            return 0;
        }

        int hint = args[0].asNumber();
        int value = args[1].asNumber();
        glfwWindowHint(hint, value);
        return 0;
    }

    int native_glfwWindowHintString(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwWindowHintString expects 2 arguments");
            return 0;
        }

        int hint = args[0].asNumber();
        const char *value = args[1].asStringChars();
        glfwWindowHintString(hint, value);
        return 0;
    }

    // =============================================================
    // WINDOW CREATION & DESTRUCTION
    // =============================================================

    int native_glfwCreateWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("glfwCreateWindow expects 5 arguments (width, height, title, monitor, share)");
            return 0;
        }

        int width = args[0].asNumber();
        int height = args[1].asNumber();
        const char *title = args[2].asStringChars();
        
        GLFWmonitor *monitor = nullptr;
        if (args[3].isPointer())
            monitor = (GLFWmonitor *)args[3].asPointer();

        GLFWwindow *share = nullptr;
        if (args[4].isPointer())
            share = (GLFWwindow *)args[4].asPointer();

        GLFWwindow *window = glfwCreateWindow(width, height, title, monitor, share);
        vm->pushPointer(window);
        return 1;
    }

    int native_glfwDestroyWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwDestroyWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwDestroyWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwDestroyWindow(window);
        return 0;
    }

    int native_glfwWindowShouldClose(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwWindowShouldClose expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwWindowShouldClose expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int result = glfwWindowShouldClose(window);
        vm->pushBool(result == GLFW_TRUE);
        return 1;
    }

    int native_glfwSetWindowShouldClose(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwSetWindowShouldClose expects 2 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowShouldClose expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int value = args[1].asBool() ? GLFW_TRUE : GLFW_FALSE;
        glfwSetWindowShouldClose(window, value);
        return 0;
    }

    // =============================================================
    // WINDOW PROPERTIES
    // =============================================================

    int native_glfwSetWindowTitle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwSetWindowTitle expects 2 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowTitle expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        const char *title = args[1].asStringChars();
        glfwSetWindowTitle(window, title);
        return 0;
    }

    int native_glfwGetWindowPos(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetWindowPos expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetWindowPos expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int x, y;
        glfwGetWindowPos(window, &x, &y);

        vm->pushInt(x);
        vm->pushInt(y);
        return 2;
    }

    int native_glfwSetWindowPos(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glfwSetWindowPos expects 3 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowPos expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int x = args[1].asNumber();
        int y = args[2].asNumber();
        glfwSetWindowPos(window, x, y);
        return 0;
    }

    int native_glfwGetWindowSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetWindowSize expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetWindowSize expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int w, h;
        glfwGetWindowSize(window, &w, &h);

        vm->pushInt(w);
        vm->pushInt(h);
        return 2;
    }

    int native_glfwSetWindowSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glfwSetWindowSize expects 3 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowSize expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int w = args[1].asNumber();
        int h = args[2].asNumber();
        glfwSetWindowSize(window, w, h);
        return 0;
    }

    int native_glfwSetWindowSizeLimits(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("glfwSetWindowSizeLimits expects 5 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowSizeLimits expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int minW = args[1].asNumber();
        int minH = args[2].asNumber();
        int maxW = args[3].asNumber();
        int maxH = args[4].asNumber();
        glfwSetWindowSizeLimits(window, minW, minH, maxW, maxH);
        return 0;
    }

    int native_glfwSetWindowAspectRatio(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glfwSetWindowAspectRatio expects 3 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowAspectRatio expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int numer = args[1].asNumber();
        int denom = args[2].asNumber();
        glfwSetWindowAspectRatio(window, numer, denom);
        return 0;
    }

    int native_glfwGetFramebufferSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetFramebufferSize expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetFramebufferSize expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        vm->pushInt(w);
        vm->pushInt(h);
        return 2;
    }

    // =============================================================
    // WINDOW VISIBILITY & STATE
    // =============================================================

    int native_glfwIconifyWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwIconifyWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwIconifyWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwIconifyWindow(window);
        return 0;
    }

    int native_glfwRestoreWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwRestoreWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwRestoreWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwRestoreWindow(window);
        return 0;
    }

    int native_glfwMaximizeWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwMaximizeWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwMaximizeWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwMaximizeWindow(window);
        return 0;
    }

    int native_glfwShowWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwShowWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwShowWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwShowWindow(window);
        return 0;
    }

    int native_glfwHideWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwHideWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwHideWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwHideWindow(window);
        return 0;
    }

    int native_glfwFocusWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwFocusWindow expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwFocusWindow expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwFocusWindow(window);
        return 0;
    }

    int native_glfwRequestWindowAttention(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwRequestWindowAttention expects 1 argument");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwRequestWindowAttention expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        glfwRequestWindowAttention(window);
        return 0;
    }

    // =============================================================
    // WINDOW ATTRIBUTES
    // =============================================================

    int native_glfwGetWindowAttrib(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwGetWindowAttrib expects 2 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetWindowAttrib expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int attrib = args[1].asNumber();
        int value = glfwGetWindowAttrib(window, attrib);
        vm->pushInt(value);
        return 1;
    }

    int native_glfwSetWindowAttrib(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glfwSetWindowAttrib expects 3 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowAttrib expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int attrib = args[1].asNumber();
        int value = args[2].asNumber();
        glfwSetWindowAttrib(window, attrib, value);
        return 0;
    }

    // =============================================================
    // EVENT PROCESSING
    // =============================================================

    int native_glfwPollEvents(Interpreter *vm, int argc, Value *args)
    {
        glfwPollEvents();
        return 0;
    }

    int native_glfwWaitEvents(Interpreter *vm, int argc, Value *args)
    {
        glfwWaitEvents();
        return 0;
    }

    int native_glfwWaitEventsTimeout(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwWaitEventsTimeout expects 1 argument");
            return 0;
        }

        double timeout = args[0].asNumber();
        glfwWaitEventsTimeout(timeout);
        return 0;
    }

    int native_glfwPostEmptyEvent(Interpreter *vm, int argc, Value *args)
    {
        glfwPostEmptyEvent();
        return 0;
    }

    void register_window(ModuleBuilder &mod)
    {
        mod
            // =============================================================
            // WINDOW HINTS
            // =============================================================
            .addFunction("glfwDefaultWindowHints", native_glfwDefaultWindowHints, 0)
            .addFunction("glfwWindowHint", native_glfwWindowHint, 2)
            .addFunction("glfwWindowHintString", native_glfwWindowHintString, 2)

            // =============================================================
            // WINDOW CREATION & DESTRUCTION
            // =============================================================
            .addFunction("glfwCreateWindow", native_glfwCreateWindow, 5)
            .addFunction("glfwDestroyWindow", native_glfwDestroyWindow, 1)
            .addFunction("glfwWindowShouldClose", native_glfwWindowShouldClose, 1)
            .addFunction("glfwSetWindowShouldClose", native_glfwSetWindowShouldClose, 2)

            // =============================================================
            // WINDOW PROPERTIES
            // =============================================================
            .addFunction("glfwSetWindowTitle", native_glfwSetWindowTitle, 2)
            .addFunction("glfwGetWindowPos", native_glfwGetWindowPos, 1)
            .addFunction("glfwSetWindowPos", native_glfwSetWindowPos, 3)
            .addFunction("glfwGetWindowSize", native_glfwGetWindowSize, 1)
            .addFunction("glfwSetWindowSize", native_glfwSetWindowSize, 3)
            .addFunction("glfwSetWindowSizeLimits", native_glfwSetWindowSizeLimits, 5)
            .addFunction("glfwSetWindowAspectRatio", native_glfwSetWindowAspectRatio, 3)
            .addFunction("glfwGetFramebufferSize", native_glfwGetFramebufferSize, 1)

            // =============================================================
            // WINDOW VISIBILITY & STATE
            // =============================================================
            .addFunction("glfwIconifyWindow", native_glfwIconifyWindow, 1)
            .addFunction("glfwRestoreWindow", native_glfwRestoreWindow, 1)
            .addFunction("glfwMaximizeWindow", native_glfwMaximizeWindow, 1)
            .addFunction("glfwShowWindow", native_glfwShowWindow, 1)
            .addFunction("glfwHideWindow", native_glfwHideWindow, 1)
            .addFunction("glfwFocusWindow", native_glfwFocusWindow, 1)
            .addFunction("glfwRequestWindowAttention", native_glfwRequestWindowAttention, 1)

            // =============================================================
            // WINDOW ATTRIBUTES
            // =============================================================
            .addFunction("glfwGetWindowAttrib", native_glfwGetWindowAttrib, 2)
            .addFunction("glfwSetWindowAttrib", native_glfwSetWindowAttrib, 3)

            // =============================================================
            // EVENT PROCESSING
            // =============================================================
            .addFunction("glfwPollEvents", native_glfwPollEvents, 0)
            .addFunction("glfwWaitEvents", native_glfwWaitEvents, 0)
            .addFunction("glfwWaitEventsTimeout", native_glfwWaitEventsTimeout, 1)
            .addFunction("glfwPostEmptyEvent", native_glfwPostEmptyEvent, 0)

            // =============================================================
            // WINDOW HINTS CONSTANTS
            // =============================================================
            .addInt("GLFW_FOCUSED", GLFW_FOCUSED)
            .addInt("GLFW_RESIZABLE", GLFW_RESIZABLE)
            .addInt("GLFW_VISIBLE", GLFW_VISIBLE)
            .addInt("GLFW_DECORATED", GLFW_DECORATED)
            .addInt("GLFW_AUTO_ICONIFY", GLFW_AUTO_ICONIFY)
            .addInt("GLFW_FLOATING", GLFW_FLOATING)
            .addInt("GLFW_MAXIMIZED", GLFW_MAXIMIZED)
            .addInt("GLFW_CENTER_CURSOR", GLFW_CENTER_CURSOR)
            .addInt("GLFW_TRANSPARENT_FRAMEBUFFER", GLFW_TRANSPARENT_FRAMEBUFFER)
            .addInt("GLFW_FOCUS_ON_SHOW", GLFW_FOCUS_ON_SHOW)
            .addInt("GLFW_SCALE_TO_MONITOR", GLFW_SCALE_TO_MONITOR)

            // =============================================================
            // FRAMEBUFFER HINTS
            // =============================================================
            .addInt("GLFW_RED_BITS", GLFW_RED_BITS)
            .addInt("GLFW_GREEN_BITS", GLFW_GREEN_BITS)
            .addInt("GLFW_BLUE_BITS", GLFW_BLUE_BITS)
            .addInt("GLFW_ALPHA_BITS", GLFW_ALPHA_BITS)
            .addInt("GLFW_DEPTH_BITS", GLFW_DEPTH_BITS)
            .addInt("GLFW_STENCIL_BITS", GLFW_STENCIL_BITS)
            .addInt("GLFW_SAMPLES", GLFW_SAMPLES)
            .addInt("GLFW_SRGB_CAPABLE", GLFW_SRGB_CAPABLE)
            .addInt("GLFW_DOUBLEBUFFER", GLFW_DOUBLEBUFFER)

            // =============================================================
            // WINDOW ATTRIBUTE CONSTANTS
            // =============================================================
            .addInt("GLFW_ICONIFIED", GLFW_ICONIFIED)
            .addInt("GLFW_HOVERED", GLFW_HOVERED)

            // =============================================================
            // SPECIAL VALUES
            // =============================================================
            .addInt("GLFW_DONT_CARE", GLFW_DONT_CARE);
    }
}