#include "bidings.hpp"

namespace RaylibBindings
{

    // ========================================
    // WINDOW
    // ========================================

    static Value native_InitWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return vm->makeNil();

        int width = args[0].asNumber();
        int height = args[1].asNumber();
        const char *title = args[2].asString()->chars();

        InitWindow(width, height, title);
        return vm->makeNil();
    }

    static Value native_CloseWindow(Interpreter *vm, int argc, Value *args)
    {
        CloseWindow();
        return vm->makeNil();
    }

    static Value native_WindowShouldClose(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(WindowShouldClose());
    }

    static Value native_IsWindowReady(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowReady());
    }

    static Value native_IsWindowFullscreen(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowFullscreen());
    }

    static Value native_IsWindowHidden(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowHidden());
    }

    static Value native_IsWindowMinimized(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowMinimized());
    }

    static Value native_IsWindowMaximized(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowMaximized());
    }

    static Value native_IsWindowFocused(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowFocused());
    }

    static Value native_IsWindowResized(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsWindowResized());
    }

    static Value native_ToggleFullscreen(Interpreter *vm, int argc, Value *args)
    {
        ToggleFullscreen();
        return vm->makeNil();
    }

    static Value native_MaximizeWindow(Interpreter *vm, int argc, Value *args)
    {
        MaximizeWindow();
        return vm->makeNil();
    }

    static Value native_MinimizeWindow(Interpreter *vm, int argc, Value *args)
    {
        MinimizeWindow();
        return vm->makeNil();
    }

    static Value native_RestoreWindow(Interpreter *vm, int argc, Value *args)
    {
        RestoreWindow();
        return vm->makeNil();
    }

    static Value native_SetWindowTitle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
        {
            Error("SetWindowTitle expects string");
            return vm->makeNil();
        }
        SetWindowTitle(args[0].asStringChars());
        return vm->makeNil();
    }

    static Value native_SetWindowPosition(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SetWindowPosition expects x, y");
            return vm->makeNil();
        }
        SetWindowPosition(args[0].asNumber(), args[1].asNumber());
        return vm->makeNil();
    }

    static Value native_SetWindowSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SetWindowSize expects width, height");
            return vm->makeNil();
        }
        SetWindowSize(args[0].asNumber(), args[1].asNumber());
        return vm->makeNil();
    }

    static Value native_SetWindowMinSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SetWindowMinSize expects width, height");
            return vm->makeNil();
        }
        SetWindowMinSize(args[0].asNumber(), args[1].asNumber());
        return vm->makeNil();
    }

    static Value native_GetScreenWidth(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetScreenWidth());
    }

    static Value native_GetScreenHeight(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetScreenHeight());
    }

    static Value native_GetRenderWidth(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetRenderWidth());
    }

    static Value native_GetRenderHeight(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetRenderHeight());
    }

    static Value native_GetMonitorCount(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetMonitorCount());
    }

    static Value native_GetCurrentMonitor(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetCurrentMonitor());
    }

    static Value native_GetMonitorWidth(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeInt(0);
        return vm->makeInt(GetMonitorWidth(args[0].asNumber()));
    }

    static Value native_GetMonitorHeight(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeInt(0);
        return vm->makeInt(GetMonitorHeight(args[0].asNumber()));
    }

    static Value native_GetMonitorRefreshRate(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeInt(0);
        return vm->makeInt(GetMonitorRefreshRate(args[0].asNumber()));
    }

    // ========================================
    // CURSOR
    // ========================================

    static Value native_ShowCursor(Interpreter *vm, int argc, Value *args)
    {
        ShowCursor();
        return vm->makeNil();
    }

    static Value native_HideCursor(Interpreter *vm, int argc, Value *args)
    {
        HideCursor();
        return vm->makeNil();
    }

    static Value native_IsCursorHidden(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsCursorHidden());
    }

    static Value native_EnableCursor(Interpreter *vm, int argc, Value *args)
    {
        EnableCursor();
        return vm->makeNil();
    }

    static Value native_DisableCursor(Interpreter *vm, int argc, Value *args)
    {
        DisableCursor();
        return vm->makeNil();
    }

    static Value native_IsCursorOnScreen(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(IsCursorOnScreen());
    }

    // ========================================
    // FPS/TIMING
    // ========================================

    static Value native_SetTargetFPS(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SetTargetFPS expects 1 argument");
            return vm->makeNil();
        }
        SetTargetFPS(args[0].asNumber());
        return vm->makeNil();
    }

    static Value native_GetFPS(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetFPS());
    }

    static Value native_GetFrameTime(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeDouble(GetFrameTime());
    }

    static Value native_GetTime(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeDouble(GetTime());
    }

    // ========================================
    // DRAWING BEGIN/END
    // ========================================

    static Value native_BeginDrawing(Interpreter *vm, int argc, Value *args)
    {
        BeginDrawing();
        return vm->makeNil();
    }

    static Value native_EndDrawing(Interpreter *vm, int argc, Value *args)
    {
        EndDrawing();
        return vm->makeNil();
    }

    static Value native_ClearBackground(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("ClearBackground expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isNativeStructInstance())
        {
            Error("ClearBackground expects Color");
            return vm->makeNil();
        }

        auto *inst = args[0].asNativeStructInstance();
        Color *c = (Color *)inst->data;

        ClearBackground(*c);
        return vm->makeNil();
    }

    // ========================================
    // SCISSOR MODE
    // ========================================

    static Value native_BeginScissorMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("BeginScissorMode expects x, y, width, height");
            return vm->makeNil();
        }
        BeginScissorMode(args[0].asNumber(), args[1].asNumber(),
                         args[2].asNumber(), args[3].asNumber());
        return vm->makeNil();
    }

    static Value native_EndScissorMode(Interpreter *vm, int argc, Value *args)
    {
        EndScissorMode();
        return vm->makeNil();
    }

    // ========================================
    // BLEND MODE
    // ========================================

    static Value native_BeginBlendMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("BeginBlendMode expects mode");
            return vm->makeNil();
        }
        BeginBlendMode(args[0].asNumber());
        return vm->makeNil();
    }

    static Value native_EndBlendMode(Interpreter *vm, int argc, Value *args)
    {
        EndBlendMode();
        return vm->makeNil();
    }

    // ========================================
    // MATH HELPERS
    // ========================================

    static Value native_Clamp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return vm->makeDouble(0);
        float value = args[0].asDouble();
        float min = args[1].asDouble();
        float max = args[2].asDouble();
        return vm->makeDouble(BuClamp(value, min, max));
    }

    static Value native_Lerp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return vm->makeDouble(0);
        float start = args[0].asDouble();
        float end = args[1].asDouble();
        float amount = args[2].asDouble();
        return vm->makeDouble(BuLerp(start, end, amount));
    }

    static Value native_GetRandomValue(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
            return vm->makeInt(0);
        return vm->makeInt(GetRandomValue(args[0].asNumber(), args[1].asNumber()));
    }

    static Value native_SetRandomSeed(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return vm->makeNil();
        SetRandomSeed(args[0].asNumber());
        return vm->makeNil();
    }

    // ========================================
    // COLOR HELPERS
    // ========================================

    static Value native_Fade(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNativeStructInstance())
        {
            Error("Fade expects Color and alpha");
            return vm->makeNil();
        }
        auto *colorInst = args[0].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;
        float alpha = args[1].asDouble();
        Color result = Fade(*color, alpha);

        // Modify original color
        color->a = result.a;
        return args[0];
    }

    // ========================================
    // REGISTER CORE
    // ========================================

    void register_core(ModuleBuilder &mod)
    {
        // Window functions
        mod.addFunction("InitWindow", native_InitWindow, 3)
           .addFunction("CloseWindow", native_CloseWindow, 0)
           .addFunction("WindowShouldClose", native_WindowShouldClose, 0)
           .addFunction("IsWindowReady", native_IsWindowReady, 0)
           .addFunction("IsWindowFullscreen", native_IsWindowFullscreen, 0)
           .addFunction("IsWindowHidden", native_IsWindowHidden, 0)
           .addFunction("IsWindowMinimized", native_IsWindowMinimized, 0)
           .addFunction("IsWindowMaximized", native_IsWindowMaximized, 0)
           .addFunction("IsWindowFocused", native_IsWindowFocused, 0)
           .addFunction("IsWindowResized", native_IsWindowResized, 0)
           .addFunction("ToggleFullscreen", native_ToggleFullscreen, 0)
           .addFunction("MaximizeWindow", native_MaximizeWindow, 0)
           .addFunction("MinimizeWindow", native_MinimizeWindow, 0)
           .addFunction("RestoreWindow", native_RestoreWindow, 0)
           .addFunction("SetWindowTitle", native_SetWindowTitle, 1)
           .addFunction("SetWindowPosition", native_SetWindowPosition, 2)
           .addFunction("SetWindowSize", native_SetWindowSize, 2)
           .addFunction("SetWindowMinSize", native_SetWindowMinSize, 2)

        // Screen functions
           .addFunction("GetScreenWidth", native_GetScreenWidth, 0)
           .addFunction("GetScreenHeight", native_GetScreenHeight, 0)
           .addFunction("GetRenderWidth", native_GetRenderWidth, 0)
           .addFunction("GetRenderHeight", native_GetRenderHeight, 0)
           .addFunction("GetMonitorCount", native_GetMonitorCount, 0)
           .addFunction("GetCurrentMonitor", native_GetCurrentMonitor, 0)
           .addFunction("GetMonitorWidth", native_GetMonitorWidth, 1)
           .addFunction("GetMonitorHeight", native_GetMonitorHeight, 1)
           .addFunction("GetMonitorRefreshRate", native_GetMonitorRefreshRate, 1)

        // Cursor functions
           .addFunction("ShowCursor", native_ShowCursor, 0)
           .addFunction("HideCursor", native_HideCursor, 0)
           .addFunction("IsCursorHidden", native_IsCursorHidden, 0)
           .addFunction("EnableCursor", native_EnableCursor, 0)
           .addFunction("DisableCursor", native_DisableCursor, 0)
           .addFunction("IsCursorOnScreen", native_IsCursorOnScreen, 0)

        // FPS/Timing
           .addFunction("SetTargetFPS", native_SetTargetFPS, 1)
           .addFunction("GetFPS", native_GetFPS, 0)
           .addFunction("GetFrameTime", native_GetFrameTime, 0)
           .addFunction("GetTime", native_GetTime, 0)

        // Drawing
           .addFunction("BeginDrawing", native_BeginDrawing, 0)
           .addFunction("EndDrawing", native_EndDrawing, 0)
           .addFunction("ClearBackground", native_ClearBackground, 1)
           .addFunction("BeginScissorMode", native_BeginScissorMode, 4)
           .addFunction("EndScissorMode", native_EndScissorMode, 0)
           .addFunction("BeginBlendMode", native_BeginBlendMode, 1)
           .addFunction("EndBlendMode", native_EndBlendMode, 0)

        // Math/Random
           .addFunction("Clamp", native_Clamp, 3)
           .addFunction("Lerp", native_Lerp, 3)
           .addFunction("GetRandomValue", native_GetRandomValue, 2)
           .addFunction("SetRandomSeed", native_SetRandomSeed, 1)

        // Color
           .addFunction("Fade", native_Fade, 2)

        // Blend mode constants
           .addInt("BLEND_ALPHA", BLEND_ALPHA)
           .addInt("BLEND_ADDITIVE", BLEND_ADDITIVE)
           .addInt("BLEND_MULTIPLIED", BLEND_MULTIPLIED)
           .addInt("BLEND_ADD_COLORS", BLEND_ADD_COLORS)
           .addInt("BLEND_SUBTRACT_COLORS", BLEND_SUBTRACT_COLORS)
           .addInt("BLEND_ALPHA_PREMULTIPLY", BLEND_ALPHA_PREMULTIPLY);
    }

}
