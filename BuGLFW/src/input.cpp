#include "bindings.hpp"
 

namespace GLFWBindings
{
    // =============================================================
    // KEYBOARD INPUT
    // =============================================================

    Value native_glfwGetKey(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwGetKey expects 2 arguments (window, key)");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetKey expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int key = args[1].asNumber();
        int state = glfwGetKey(window, key);
        return vm->makeInt(state);
    }

    Value native_glfwGetKeyScancode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetKeyScancode expects 1 argument");
            return vm->makeNil();
        }

        int key = args[0].asNumber();
        int scancode = glfwGetKeyScancode(key);
        return vm->makeInt(scancode);
    }

    Value native_glfwGetKeyName(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwGetKeyName expects 2 arguments");
            return vm->makeNil();
        }

        int key = args[0].asNumber();
        int scancode = args[1].asNumber();
        const char *name = glfwGetKeyName(key, scancode);
        return name ? vm->makeString(name) : vm->makeNil();
    }

    // =============================================================
    // MOUSE INPUT
    // =============================================================

    Value native_glfwGetMouseButton(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwGetMouseButton expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetMouseButton expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int button = args[1].asNumber();
        int state = glfwGetMouseButton(window, button);
        return vm->makeInt(state);
    }

    Value native_glfwGetCursorPos(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetCursorPos expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetCursorPos expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("x").asString(), vm->makeDouble(x));
        map->table.set(vm->makeString("y").asString(), vm->makeDouble(y));
        return result;
    }

    Value native_glfwSetCursorPos(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glfwSetCursorPos expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetCursorPos expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        double x = args[1].asNumber();
        double y = args[2].asNumber();
        glfwSetCursorPos(window, x, y);
        return vm->makeNil();
    }

    // =============================================================
    // INPUT MODE
    // =============================================================

    Value native_glfwGetInputMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("glfwGetInputMode expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwGetInputMode expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int mode = args[1].asNumber();
        int value = glfwGetInputMode(window, mode);
        return vm->makeInt(value);
    }

    Value native_glfwSetInputMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("glfwSetInputMode expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("glfwSetInputMode expects window pointer");
            return vm->makeNil();
        }

        GLFWwindow *window = (GLFWwindow *)args[0].asPointer();
        int mode = args[1].asNumber();
        int value = args[2].asNumber();
        glfwSetInputMode(window, mode, value);
        return vm->makeNil();
    }

    // =============================================================
    // JOYSTICK INPUT
    // =============================================================

    Value native_glfwJoystickPresent(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwJoystickPresent expects 1 argument");
            return vm->makeNil();
        }

        int jid = args[0].asNumber();
        int present = glfwJoystickPresent(jid);
        return vm->makeBool(present == GLFW_TRUE);
    }

    Value native_glfwGetJoystickName(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetJoystickName expects 1 argument");
            return vm->makeNil();
        }

        int jid = args[0].asNumber();
        const char *name = glfwGetJoystickName(jid);
        return name ? vm->makeString(name) : vm->makeNil();
    }

    Value native_glfwJoystickIsGamepad(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwJoystickIsGamepad expects 1 argument");
            return vm->makeNil();
        }

        int jid = args[0].asNumber();
        int result = glfwJoystickIsGamepad(jid);
        return vm->makeBool(result == GLFW_TRUE);
    }

    Value native_glfwGetGamepadName(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("glfwGetGamepadName expects 1 argument");
            return vm->makeNil();
        }

        int jid = args[0].asNumber();
        const char *name = glfwGetGamepadName(jid);
        return name ? vm->makeString(name) : vm->makeNil();
    }

    void register_input(ModuleBuilder &mod)
    {
        mod
            // =============================================================
            // KEYBOARD INPUT
            // =============================================================
            .addFunction("glfwGetKey", native_glfwGetKey, 2)
            .addFunction("glfwGetKeyScancode", native_glfwGetKeyScancode, 1)
            .addFunction("glfwGetKeyName", native_glfwGetKeyName, 2)

            // =============================================================
            // MOUSE INPUT
            // =============================================================
            .addFunction("glfwGetMouseButton", native_glfwGetMouseButton, 2)
            .addFunction("glfwGetCursorPos", native_glfwGetCursorPos, 1)
            .addFunction("glfwSetCursorPos", native_glfwSetCursorPos, 3)

            // =============================================================
            // INPUT MODE
            // =============================================================
            .addFunction("glfwGetInputMode", native_glfwGetInputMode, 2)
            .addFunction("glfwSetInputMode", native_glfwSetInputMode, 3)

            // =============================================================
            // JOYSTICK INPUT
            // =============================================================
            .addFunction("glfwJoystickPresent", native_glfwJoystickPresent, 1)
            .addFunction("glfwGetJoystickName", native_glfwGetJoystickName, 1)
            .addFunction("glfwJoystickIsGamepad", native_glfwJoystickIsGamepad, 1)
            .addFunction("glfwGetGamepadName", native_glfwGetGamepadName, 1)

            // =============================================================
            // KEY STATES
            // =============================================================
            .addByte("GLFW_RELEASE", GLFW_RELEASE)
            .addByte("GLFW_PRESS", GLFW_PRESS)
            .addByte("GLFW_REPEAT", GLFW_REPEAT)

            // =============================================================
            // INPUT MODES
            // =============================================================
            .addInt("GLFW_CURSOR", GLFW_CURSOR)
            .addInt("GLFW_STICKY_KEYS", GLFW_STICKY_KEYS)
            .addInt("GLFW_STICKY_MOUSE_BUTTONS", GLFW_STICKY_MOUSE_BUTTONS)
            .addInt("GLFW_LOCK_KEY_MODS", GLFW_LOCK_KEY_MODS)
            .addInt("GLFW_RAW_MOUSE_MOTION", GLFW_RAW_MOUSE_MOTION)

            // =============================================================
            // CURSOR MODES
            // =============================================================
            .addInt("GLFW_CURSOR_NORMAL", GLFW_CURSOR_NORMAL)
            .addInt("GLFW_CURSOR_HIDDEN", GLFW_CURSOR_HIDDEN)
            .addInt("GLFW_CURSOR_DISABLED", GLFW_CURSOR_DISABLED)

            // =============================================================
            // MOUSE BUTTONS
            // =============================================================
            .addByte("GLFW_MOUSE_BUTTON_1", GLFW_MOUSE_BUTTON_1)
            .addByte("GLFW_MOUSE_BUTTON_2", GLFW_MOUSE_BUTTON_2)
            .addByte("GLFW_MOUSE_BUTTON_3", GLFW_MOUSE_BUTTON_3)
            .addByte("GLFW_MOUSE_BUTTON_4", GLFW_MOUSE_BUTTON_4)
            .addByte("GLFW_MOUSE_BUTTON_5", GLFW_MOUSE_BUTTON_5)
            .addByte("GLFW_MOUSE_BUTTON_6", GLFW_MOUSE_BUTTON_6)
            .addByte("GLFW_MOUSE_BUTTON_7", GLFW_MOUSE_BUTTON_7)
            .addByte("GLFW_MOUSE_BUTTON_8", GLFW_MOUSE_BUTTON_8)
            .addByte("GLFW_MOUSE_BUTTON_LEFT", GLFW_MOUSE_BUTTON_LEFT)
            .addByte("GLFW_MOUSE_BUTTON_RIGHT", GLFW_MOUSE_BUTTON_RIGHT)
            .addByte("GLFW_MOUSE_BUTTON_MIDDLE", GLFW_MOUSE_BUTTON_MIDDLE)

            // =============================================================
            // JOYSTICK IDS
            // =============================================================
            .addByte("GLFW_JOYSTICK_1", GLFW_JOYSTICK_1)
            .addByte("GLFW_JOYSTICK_2", GLFW_JOYSTICK_2)
            .addByte("GLFW_JOYSTICK_3", GLFW_JOYSTICK_3)
            .addByte("GLFW_JOYSTICK_4", GLFW_JOYSTICK_4)
            .addByte("GLFW_JOYSTICK_5", GLFW_JOYSTICK_5)
            .addByte("GLFW_JOYSTICK_6", GLFW_JOYSTICK_6)
            .addByte("GLFW_JOYSTICK_7", GLFW_JOYSTICK_7)
            .addByte("GLFW_JOYSTICK_8", GLFW_JOYSTICK_8)
            .addByte("GLFW_JOYSTICK_9", GLFW_JOYSTICK_9)
            .addByte("GLFW_JOYSTICK_10", GLFW_JOYSTICK_10)
            .addByte("GLFW_JOYSTICK_11", GLFW_JOYSTICK_11)
            .addByte("GLFW_JOYSTICK_12", GLFW_JOYSTICK_12)
            .addByte("GLFW_JOYSTICK_13", GLFW_JOYSTICK_13)
            .addByte("GLFW_JOYSTICK_14", GLFW_JOYSTICK_14)
            .addByte("GLFW_JOYSTICK_15", GLFW_JOYSTICK_15)
            .addByte("GLFW_JOYSTICK_16", GLFW_JOYSTICK_16)

            // =============================================================
            // KEYBOARD KEYS (Common ones)
            // =============================================================
            .addInt("GLFW_KEY_SPACE", GLFW_KEY_SPACE)
            .addInt("GLFW_KEY_APOSTROPHE", GLFW_KEY_APOSTROPHE)
            .addInt("GLFW_KEY_COMMA", GLFW_KEY_COMMA)
            .addInt("GLFW_KEY_MINUS", GLFW_KEY_MINUS)
            .addInt("GLFW_KEY_PERIOD", GLFW_KEY_PERIOD)
            .addInt("GLFW_KEY_SLASH", GLFW_KEY_SLASH)
            .addInt("GLFW_KEY_0", GLFW_KEY_0)
            .addInt("GLFW_KEY_1", GLFW_KEY_1)
            .addInt("GLFW_KEY_2", GLFW_KEY_2)
            .addInt("GLFW_KEY_3", GLFW_KEY_3)
            .addInt("GLFW_KEY_4", GLFW_KEY_4)
            .addInt("GLFW_KEY_5", GLFW_KEY_5)
            .addInt("GLFW_KEY_6", GLFW_KEY_6)
            .addInt("GLFW_KEY_7", GLFW_KEY_7)
            .addInt("GLFW_KEY_8", GLFW_KEY_8)
            .addInt("GLFW_KEY_9", GLFW_KEY_9)
            .addInt("GLFW_KEY_SEMICOLON", GLFW_KEY_SEMICOLON)
            .addInt("GLFW_KEY_EQUAL", GLFW_KEY_EQUAL)
            .addInt("GLFW_KEY_A", GLFW_KEY_A)
            .addInt("GLFW_KEY_B", GLFW_KEY_B)
            .addInt("GLFW_KEY_C", GLFW_KEY_C)
            .addInt("GLFW_KEY_D", GLFW_KEY_D)
            .addInt("GLFW_KEY_E", GLFW_KEY_E)
            .addInt("GLFW_KEY_F", GLFW_KEY_F)
            .addInt("GLFW_KEY_G", GLFW_KEY_G)
            .addInt("GLFW_KEY_H", GLFW_KEY_H)
            .addInt("GLFW_KEY_I", GLFW_KEY_I)
            .addInt("GLFW_KEY_J", GLFW_KEY_J)
            .addInt("GLFW_KEY_K", GLFW_KEY_K)
            .addInt("GLFW_KEY_L", GLFW_KEY_L)
            .addInt("GLFW_KEY_M", GLFW_KEY_M)
            .addInt("GLFW_KEY_N", GLFW_KEY_N)
            .addInt("GLFW_KEY_O", GLFW_KEY_O)
            .addInt("GLFW_KEY_P", GLFW_KEY_P)
            .addInt("GLFW_KEY_Q", GLFW_KEY_Q)
            .addInt("GLFW_KEY_R", GLFW_KEY_R)
            .addInt("GLFW_KEY_S", GLFW_KEY_S)
            .addInt("GLFW_KEY_T", GLFW_KEY_T)
            .addInt("GLFW_KEY_U", GLFW_KEY_U)
            .addInt("GLFW_KEY_V", GLFW_KEY_V)
            .addInt("GLFW_KEY_W", GLFW_KEY_W)
            .addInt("GLFW_KEY_X", GLFW_KEY_X)
            .addInt("GLFW_KEY_Y", GLFW_KEY_Y)
            .addInt("GLFW_KEY_Z", GLFW_KEY_Z)
            .addInt("GLFW_KEY_ESCAPE", GLFW_KEY_ESCAPE)
            .addInt("GLFW_KEY_ENTER", GLFW_KEY_ENTER)
            .addInt("GLFW_KEY_TAB", GLFW_KEY_TAB)
            .addInt("GLFW_KEY_BACKSPACE", GLFW_KEY_BACKSPACE)
            .addInt("GLFW_KEY_INSERT", GLFW_KEY_INSERT)
            .addInt("GLFW_KEY_DELETE", GLFW_KEY_DELETE)
            .addInt("GLFW_KEY_RIGHT", GLFW_KEY_RIGHT)
            .addInt("GLFW_KEY_LEFT", GLFW_KEY_LEFT)
            .addInt("GLFW_KEY_DOWN", GLFW_KEY_DOWN)
            .addInt("GLFW_KEY_UP", GLFW_KEY_UP)
            .addInt("GLFW_KEY_PAGE_UP", GLFW_KEY_PAGE_UP)
            .addInt("GLFW_KEY_PAGE_DOWN", GLFW_KEY_PAGE_DOWN)
            .addInt("GLFW_KEY_HOME", GLFW_KEY_HOME)
            .addInt("GLFW_KEY_END", GLFW_KEY_END)
            .addInt("GLFW_KEY_CAPS_LOCK", GLFW_KEY_CAPS_LOCK)
            .addInt("GLFW_KEY_SCROLL_LOCK", GLFW_KEY_SCROLL_LOCK)
            .addInt("GLFW_KEY_NUM_LOCK", GLFW_KEY_NUM_LOCK)
            .addInt("GLFW_KEY_PRINT_SCREEN", GLFW_KEY_PRINT_SCREEN)
            .addInt("GLFW_KEY_PAUSE", GLFW_KEY_PAUSE)
            .addInt("GLFW_KEY_F1", GLFW_KEY_F1)
            .addInt("GLFW_KEY_F2", GLFW_KEY_F2)
            .addInt("GLFW_KEY_F3", GLFW_KEY_F3)
            .addInt("GLFW_KEY_F4", GLFW_KEY_F4)
            .addInt("GLFW_KEY_F5", GLFW_KEY_F5)
            .addInt("GLFW_KEY_F6", GLFW_KEY_F6)
            .addInt("GLFW_KEY_F7", GLFW_KEY_F7)
            .addInt("GLFW_KEY_F8", GLFW_KEY_F8)
            .addInt("GLFW_KEY_F9", GLFW_KEY_F9)
            .addInt("GLFW_KEY_F10", GLFW_KEY_F10)
            .addInt("GLFW_KEY_F11", GLFW_KEY_F11)
            .addInt("GLFW_KEY_F12", GLFW_KEY_F12)
            .addInt("GLFW_KEY_LEFT_SHIFT", GLFW_KEY_LEFT_SHIFT)
            .addInt("GLFW_KEY_LEFT_CONTROL", GLFW_KEY_LEFT_CONTROL)
            .addInt("GLFW_KEY_LEFT_ALT", GLFW_KEY_LEFT_ALT)
            .addInt("GLFW_KEY_LEFT_SUPER", GLFW_KEY_LEFT_SUPER)
            .addInt("GLFW_KEY_RIGHT_SHIFT", GLFW_KEY_RIGHT_SHIFT)
            .addInt("GLFW_KEY_RIGHT_CONTROL", GLFW_KEY_RIGHT_CONTROL)
            .addInt("GLFW_KEY_RIGHT_ALT", GLFW_KEY_RIGHT_ALT)
            .addInt("GLFW_KEY_RIGHT_SUPER", GLFW_KEY_RIGHT_SUPER)

            // =============================================================
            // MODIFIER KEY FLAGS
            // =============================================================
            .addByte("GLFW_MOD_SHIFT", GLFW_MOD_SHIFT)
            .addByte("GLFW_MOD_CONTROL", GLFW_MOD_CONTROL)
            .addByte("GLFW_MOD_ALT", GLFW_MOD_ALT)
            .addByte("GLFW_MOD_SUPER", GLFW_MOD_SUPER)
            .addByte("GLFW_MOD_CAPS_LOCK", GLFW_MOD_CAPS_LOCK)
            .addByte("GLFW_MOD_NUM_LOCK", GLFW_MOD_NUM_LOCK);
    }
}