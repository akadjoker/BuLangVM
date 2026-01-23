#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // KEYBOARD
    // ========================================

    static Value native_IsKeyPressed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeBool(false);
        return vm->makeBool(IsKeyPressed((int)args[0].asNumber()));
    }

    static Value native_IsKeyDown(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeBool(false);
        return vm->makeBool(IsKeyDown((int)args[0].asNumber()));
    }

    static Value native_IsKeyReleased(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeBool(false);
        return vm->makeBool(IsKeyReleased((int)args[0].asNumber()));
    }

    static Value native_IsKeyUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeBool(false);
        return vm->makeBool(IsKeyUp((int)args[0].asNumber()));
    }

    static Value native_GetKeyPressed(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetKeyPressed());
    }

    // ========================================
    // MOUSE
    // ========================================

    static Value native_IsMouseButtonPressed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("IsMouseButtonPressed expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isInt())
        {
            Error("IsMouseButtonPressed expects int");
            return vm->makeNil();
        }

        return vm->makeBool(IsMouseButtonPressed(args[0].asNumber()));
    }

    static Value native_IsMouseButtonDown(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("IsMouseButtonDown expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isInt())
        {
            Error("IsMouseButtonDown expects int");
            return vm->makeNil();
        }

        return vm->makeBool(IsMouseButtonDown(args[0].asNumber()));
    }

    static Value native_IsMouseButtonReleased(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("IsMouseButtonReleased expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isInt())
        {
            Error("IsMouseButtonReleased expects int");
            return vm->makeNil();
        }

        return vm->makeBool(IsMouseButtonReleased(args[0].asNumber()));
    }

    static Value native_IsMouseButtonUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("IsMouseButtonUp expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isInt())
        {
            Error("IsMouseButtonUp expects int");
            return vm->makeNil();
        }

        return vm->makeBool(IsMouseButtonUp(args[0].asNumber()));
    }

    static Value native_GetMouseX(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetMouseX());
    }

    static Value native_GetMouseY(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetMouseY());
    }

    static Value native_GetMousePosition(Interpreter *vm, int argc, Value *args)
    {
        Vector2 v = GetMousePosition();
        // TODO: Return Vector2 struct instance
        return vm->makeInt(-1);
    }

    // ========================================
    // REGISTER INPUT
    // ========================================

    void register_input(ModuleBuilder &mod)
    {
        // Keyboard functions
        mod.addFunction("IsKeyPressed", native_IsKeyPressed, 1)
           .addFunction("IsKeyDown", native_IsKeyDown, 1)
           .addFunction("IsKeyReleased", native_IsKeyReleased, 1)
           .addFunction("IsKeyUp", native_IsKeyUp, 1)
           .addFunction("GetKeyPressed", native_GetKeyPressed, 0)

        // Mouse functions
           .addFunction("IsMouseButtonPressed", native_IsMouseButtonPressed, 1)
           .addFunction("IsMouseButtonDown", native_IsMouseButtonDown, 1)
           .addFunction("IsMouseButtonReleased", native_IsMouseButtonReleased, 1)
           .addFunction("IsMouseButtonUp", native_IsMouseButtonUp, 1)
           .addFunction("GetMouseX", native_GetMouseX, 0)
           .addFunction("GetMouseY", native_GetMouseY, 0)

        // Key constants
           .addInt("KEY_SPACE", KEY_SPACE)
           .addInt("KEY_ESCAPE", KEY_ESCAPE)
           .addInt("KEY_ENTER", KEY_ENTER)
           .addInt("KEY_TAB", KEY_TAB)
           .addInt("KEY_BACKSPACE", KEY_BACKSPACE)
           .addInt("KEY_RIGHT", KEY_RIGHT)
           .addInt("KEY_LEFT", KEY_LEFT)
           .addInt("KEY_DOWN", KEY_DOWN)
           .addInt("KEY_UP", KEY_UP)
           .addInt("KEY_A", KEY_A)
           .addInt("KEY_B", KEY_B)
           .addInt("KEY_C", KEY_C)
           .addInt("KEY_D", KEY_D)
           .addInt("KEY_E", KEY_E)
           .addInt("KEY_F", KEY_F)
           .addInt("KEY_G", KEY_G)
           .addInt("KEY_H", KEY_H)
           .addInt("KEY_I", KEY_I)
           .addInt("KEY_J", KEY_J)
           .addInt("KEY_K", KEY_K)
           .addInt("KEY_L", KEY_L)
           .addInt("KEY_M", KEY_M)
           .addInt("KEY_N", KEY_N)
           .addInt("KEY_O", KEY_O)
           .addInt("KEY_P", KEY_P)
           .addInt("KEY_Q", KEY_Q)
           .addInt("KEY_R", KEY_R)
           .addInt("KEY_S", KEY_S)
           .addInt("KEY_T", KEY_T)
           .addInt("KEY_U", KEY_U)
           .addInt("KEY_V", KEY_V)
           .addInt("KEY_W", KEY_W)
           .addInt("KEY_X", KEY_X)
           .addInt("KEY_Y", KEY_Y)
           .addInt("KEY_Z", KEY_Z)
           .addInt("KEY_ZERO", KEY_ZERO)
           .addInt("KEY_ONE", KEY_ONE)
           .addInt("KEY_TWO", KEY_TWO)
           .addInt("KEY_THREE", KEY_THREE)
           .addInt("KEY_FOUR", KEY_FOUR)
           .addInt("KEY_FIVE", KEY_FIVE)
           .addInt("KEY_SIX", KEY_SIX)
           .addInt("KEY_SEVEN", KEY_SEVEN)
           .addInt("KEY_EIGHT", KEY_EIGHT)
           .addInt("KEY_NINE", KEY_NINE)

        // Mouse button constants
           .addInt("MOUSE_BUTTON_LEFT", MOUSE_BUTTON_LEFT)
           .addInt("MOUSE_BUTTON_RIGHT", MOUSE_BUTTON_RIGHT)
           .addInt("MOUSE_BUTTON_MIDDLE", MOUSE_BUTTON_MIDDLE);
    }

}
