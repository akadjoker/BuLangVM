#include "bindings.hpp"

namespace GLFWBindings
{
    // =============================================================
    // MONITOR FUNCTIONS
    // =============================================================

    int native_glfwGetMonitors(Interpreter *vm, int argc, Value *args)
    {
        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);

        Value result = vm->makeArray();
        ArrayInstance *arr = result.asArray();

        for (int i = 0; i < count; i++)
        {
            arr->values.push(vm->makePointer(monitors[i]));
        }

        vm->push(result);
        return 1;
    }

    int native_glfwGetPrimaryMonitor(Interpreter *vm, int argc, Value *args)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        vm->push(vm->makePointer(monitor));
        return 1;
    }

    int native_glfwGetMonitorPos(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorPos expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int x, y;
        glfwGetMonitorPos(monitor, &x, &y);
        vm->pushInt(x);
        vm->pushInt(y);
        return 2;

    }

    int native_glfwGetMonitorWorkarea(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorWorkarea expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int x, y, w, h;
        glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);

        vm->pushInt(x);
        vm->pushInt(y);
        vm->pushInt(w);
        vm->pushInt(h);
        return 4;
    }

    int native_glfwGetMonitorPhysicalSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorPhysicalSize expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int w, h;
        glfwGetMonitorPhysicalSize(monitor, &w, &h);

        vm->pushInt(w);
        vm->pushInt(h);
        return 2;
    }

    int native_glfwGetMonitorContentScale(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorContentScale expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        float x, y;
        glfwGetMonitorContentScale(monitor, &x, &y);

        vm->pushFloat(x);
        vm->pushFloat(y);
        return 2;
    }

    int native_glfwGetMonitorName(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorName expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        const char *name = glfwGetMonitorName(monitor);

        vm->pushString(name);
        return 1;
    }

    int native_glfwGetVideoMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetVideoMode expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        if (!mode)
            return 0;

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("width").asString(), vm->makeInt(mode->width));
        map->table.set(vm->makeString("height").asString(), vm->makeInt(mode->height));
        map->table.set(vm->makeString("redBits").asString(), vm->makeInt(mode->redBits));
        map->table.set(vm->makeString("greenBits").asString(), vm->makeInt(mode->greenBits));
        map->table.set(vm->makeString("blueBits").asString(), vm->makeInt(mode->blueBits));
        map->table.set(vm->makeString("refreshRate").asString(), vm->makeInt(mode->refreshRate));
        
        vm->push(result);
        return 1;
    }

    int native_glfwGetVideoModes(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetVideoModes expects monitor pointer");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int count;
        const GLFWvidmode *modes = glfwGetVideoModes(monitor, &count);



        Value result = vm->makeArray();
        ArrayInstance *arr = result.asArray();

        for (int i = 0; i < count; i++)
        {
            Value mode = vm->makeMap();
            MapInstance *map = mode.asMap();
            map->table.set(vm->makeString("width").asString(), vm->makeInt(modes[i].width));
            map->table.set(vm->makeString("height").asString(), vm->makeInt(modes[i].height));
            map->table.set(vm->makeString("refreshRate").asString(), vm->makeInt(modes[i].refreshRate));
            arr->values.push(mode);
        }

        vm->push(result);
        return 1;
    }

    int native_glfwSetGamma(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
        {
            Error("glfwSetGamma expects monitor pointer and gamma value");
            return 0;
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        float gamma = args[1].asDouble();
        glfwSetGamma(monitor, gamma);
        return 0;
    }

    int native_glfwGetWindowMonitor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetWindowMonitor expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        GLFWmonitor *monitor = glfwGetWindowMonitor(window);
        vm->pushPointer(monitor);

        return 1;
    }

    int native_glfwSetWindowMonitor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("glfwSetWindowMonitor expects 7 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowMonitor expects window pointer");
            return 0;
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        GLFWmonitor *monitor = args[1].isPointer() ? (GLFWmonitor *)args[1].asPointer() : nullptr;
        int x = args[2].asNumber();
        int y = args[3].asNumber();
        int w = args[4].asNumber();
        int h = args[5].asNumber();
        int refreshRate = args[6].asNumber();

        glfwSetWindowMonitor(window, monitor, x, y, w, h, refreshRate);
        return 0;
    }

    // =============================================================
    // REGISTER MONITOR
    // =============================================================

    void register_monitor(ModuleBuilder &mod)
    {
        mod
            .addFunction("glfwGetMonitors", native_glfwGetMonitors, 0)
            .addFunction("glfwGetPrimaryMonitor", native_glfwGetPrimaryMonitor, 0)
            .addFunction("glfwGetMonitorPos", native_glfwGetMonitorPos, 1)
            .addFunction("glfwGetMonitorWorkarea", native_glfwGetMonitorWorkarea, 1)
            .addFunction("glfwGetMonitorPhysicalSize", native_glfwGetMonitorPhysicalSize, 1)
            .addFunction("glfwGetMonitorContentScale", native_glfwGetMonitorContentScale, 1)
            .addFunction("glfwGetMonitorName", native_glfwGetMonitorName, 1)
            .addFunction("glfwGetVideoMode", native_glfwGetVideoMode, 1)
            .addFunction("glfwGetVideoModes", native_glfwGetVideoModes, 1)
            .addFunction("glfwSetGamma", native_glfwSetGamma, 2)
            .addFunction("glfwGetWindowMonitor", native_glfwGetWindowMonitor, 1)
            .addFunction("glfwSetWindowMonitor", native_glfwSetWindowMonitor, 7);
    }
}
