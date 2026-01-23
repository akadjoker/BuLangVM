#include "bindings.hpp"

namespace GLFWBindings
{
    // =============================================================
    // MONITOR FUNCTIONS
    // =============================================================

    Value native_glfwGetMonitors(Interpreter *vm, int argc, Value *args)
    {
        int count;
        GLFWmonitor **monitors = glfwGetMonitors(&count);

        Value result = vm->makeArray();
        ArrayInstance *arr = result.asArray();

        for (int i = 0; i < count; i++)
        {
            arr->values.push(vm->makePointer(monitors[i]));
        }

        return result;
    }

    Value native_glfwGetPrimaryMonitor(Interpreter *vm, int argc, Value *args)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        return monitor ? vm->makePointer(monitor) : vm->makeNil();
    }

    Value native_glfwGetMonitorPos(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorPos expects monitor pointer");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int x, y;
        glfwGetMonitorPos(monitor, &x, &y);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("x").asString(), vm->makeInt(x));
        map->table.set(vm->makeString("y").asString(), vm->makeInt(y));
        return result;
    }

    Value native_glfwGetMonitorWorkarea(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorWorkarea expects monitor pointer");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int x, y, w, h;
        glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("x").asString(), vm->makeInt(x));
        map->table.set(vm->makeString("y").asString(), vm->makeInt(y));
        map->table.set(vm->makeString("w").asString(), vm->makeInt(w));
        map->table.set(vm->makeString("h").asString(), vm->makeInt(h));
        return result;
    }

    Value native_glfwGetMonitorPhysicalSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorPhysicalSize expects monitor pointer");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        int w, h;
        glfwGetMonitorPhysicalSize(monitor, &w, &h);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("w").asString(), vm->makeInt(w));
        map->table.set(vm->makeString("h").asString(), vm->makeInt(h));
        return result;
    }

    Value native_glfwGetMonitorContentScale(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorContentScale expects monitor pointer");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        float x, y;
        glfwGetMonitorContentScale(monitor, &x, &y);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("x").asString(), vm->makeDouble(x));
        map->table.set(vm->makeString("y").asString(), vm->makeDouble(y));
        return result;
    }

    Value native_glfwGetMonitorName(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetMonitorName expects monitor pointer");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        const char *name = glfwGetMonitorName(monitor);
        return name ? vm->makeString(name) : vm->makeString("");
    }

    Value native_glfwGetVideoMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetVideoMode expects monitor pointer");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        if (!mode)
            return vm->makeNil();

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("width").asString(), vm->makeInt(mode->width));
        map->table.set(vm->makeString("height").asString(), vm->makeInt(mode->height));
        map->table.set(vm->makeString("redBits").asString(), vm->makeInt(mode->redBits));
        map->table.set(vm->makeString("greenBits").asString(), vm->makeInt(mode->greenBits));
        map->table.set(vm->makeString("blueBits").asString(), vm->makeInt(mode->blueBits));
        map->table.set(vm->makeString("refreshRate").asString(), vm->makeInt(mode->refreshRate));
        return result;
    }

    Value native_glfwGetVideoModes(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetVideoModes expects monitor pointer");
            return vm->makeNil();
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

        return result;
    }

    Value native_glfwSetGamma(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
        {
            Error("glfwSetGamma expects monitor pointer and gamma value");
            return vm->makeNil();
        }

        GLFWmonitor *monitor = (GLFWmonitor *)args[0].asPointer();
        float gamma = args[1].asDouble();
        glfwSetGamma(monitor, gamma);
        return vm->makeNil();
    }

    Value native_glfwGetWindowMonitor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("glfwGetWindowMonitor expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        GLFWmonitor *monitor = glfwGetWindowMonitor(window);
        return monitor ? vm->makePointer(monitor) : vm->makeNil();
    }

    Value native_glfwSetWindowMonitor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("glfwSetWindowMonitor expects 7 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetWindowMonitor expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        GLFWmonitor *monitor = args[1].isPointer() ? (GLFWmonitor *)args[1].asPointer() : nullptr;
        int x = args[2].asNumber();
        int y = args[3].asNumber();
        int w = args[4].asNumber();
        int h = args[5].asNumber();
        int refreshRate = args[6].asNumber();

        glfwSetWindowMonitor(window, monitor, x, y, w, h, refreshRate);
        return vm->makeNil();
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
