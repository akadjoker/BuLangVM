#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // WINDOW
    // ========================================

    static int native_InitWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
            return 0;

        int width = args[0].asNumber();
        int height = args[1].asNumber();
        const char *title = args[2].asString()->chars();

        InitWindow(width, height, title);
        return 0;
    }

    static int native_CloseWindow(Interpreter *vm, int argc, Value *args)
    {
        CloseWindow();
        return 0;
    }

    static int native_WindowShouldClose(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(WindowShouldClose());
        return 1;
    }

    static int native_IsWindowReady(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowReady());
        return 1;
    }

    static int native_IsWindowFullscreen(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowFullscreen());
        return 1;
    }

    static int native_IsWindowHidden(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowHidden());
        return 1;
    }

    static int native_IsWindowMinimized(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowMinimized());
        return 1;
    }

    static int native_IsWindowMaximized(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowMaximized());
        return 1;
    }

    static int native_IsWindowFocused(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowFocused());
        return 1;
    }

    static int native_IsWindowResized(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsWindowResized());
        return 1;
    }

    static int native_ToggleFullscreen(Interpreter *vm, int argc, Value *args)
    {
        ToggleFullscreen();
        return 0;
    }

    static int native_MaximizeWindow(Interpreter *vm, int argc, Value *args)
    {
        MaximizeWindow();
        return 0;
    }

    static int native_MinimizeWindow(Interpreter *vm, int argc, Value *args)
    {
        MinimizeWindow();
        return 0;
    }

    static int native_RestoreWindow(Interpreter *vm, int argc, Value *args)
    {
        RestoreWindow();
        return 0;
    }

    static int native_SetWindowTitle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
        {
            Error("SetWindowTitle expects string");
            return 0;
        }
        SetWindowTitle(args[0].asStringChars());
        return 0;
    }

    static int native_SetWindowPosition(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SetWindowPosition expects x, y");
            return 0;
        }
        SetWindowPosition(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    static int native_SetWindowSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SetWindowSize expects width, height");
            return 0;
        }
        SetWindowSize(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    static int native_SetWindowMinSize(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SetWindowMinSize expects width, height");
            return 0;
        }
        SetWindowMinSize(args[0].asNumber(), args[1].asNumber());
        return 0;
    }

    static int native_GetScreenWidth(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetScreenWidth());
        return 1;
    }

    static int native_GetScreenHeight(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetScreenHeight());
        return 1;
    }

    static int native_GetRenderWidth(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetRenderWidth());
        return 1;
    }

    static int native_GetRenderHeight(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetRenderHeight());
        return 1;
    }

    static int native_GetMonitorCount(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetMonitorCount());
        return 1;
    }

    static int native_GetCurrentMonitor(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetCurrentMonitor());
        return 1;
    }

    static int native_GetMonitorWidth(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushInt(GetMonitorWidth(args[0].asNumber()));
        return 1;
    }

    static int native_GetMonitorHeight(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushInt(GetMonitorHeight(args[0].asNumber()));
        return 1;
    }

    static int native_GetMonitorRefreshRate(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
            return 0;
        vm->pushInt(GetMonitorRefreshRate(args[0].asNumber()));
        return 1;
    }


    // ========================================
    // CURSOR
    // ========================================

    static int native_ShowCursor(Interpreter *vm, int argc, Value *args)
    {
        ShowCursor();
        return 0;
    }

    static int native_HideCursor(Interpreter *vm, int argc, Value *args)
    {
        HideCursor();
        return 0;
    }

    static int native_IsCursorHidden(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsCursorHidden());
        return 1;
    }

    static int native_EnableCursor(Interpreter *vm, int argc, Value *args)
    {
        EnableCursor();
        return 0;
    }

    static int native_DisableCursor(Interpreter *vm, int argc, Value *args)
    {
        DisableCursor();
        return 0;
    }

    static int native_IsCursorOnScreen(Interpreter *vm, int argc, Value *args)
    {
        vm->pushBool(IsCursorOnScreen());
        return 1;
    }

    // ========================================
    // FPS/TIMING
    // ========================================

    static int native_SetTargetFPS(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SetTargetFPS expects 1 argument");
            return 0;
        }
        SetTargetFPS(args[0].asNumber());
        return 0;
    }

    static int native_GetFPS(Interpreter *vm, int argc, Value *args)
    {
        vm->pushInt(GetFPS());
        return 1;
    }

    static int native_GetFrameTime(Interpreter *vm, int argc, Value *args)
    {
        vm->pushDouble(GetFrameTime());
        return 1;
    }

    static int native_GetTime(Interpreter *vm, int argc, Value *args)
    {
        vm->pushDouble(GetTime());
        return 1;
    }

    // ========================================
    // DRAWING BEGIN/END
    // ========================================

    static int native_BeginDrawing(Interpreter *vm, int argc, Value *args)
    {
        BeginDrawing();
        return 0;
    }

    static int native_EndDrawing(Interpreter *vm, int argc, Value *args)
    {
        EndDrawing();
        return 0;
    }

    static int native_ClearBackground(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("ClearBackground expects 1 argument");
            return 0;
        }
        if (!args[0].isNativeStructInstance())
        {
            Error("ClearBackground expects Color");
            return 0;
        }

        auto *inst = args[0].asNativeStructInstance();
        Color *c = (Color *)inst->data;

        ClearBackground(*c);
        return 0;
    }

    // ========================================
    // SCISSOR MODE
    // ========================================

    static int native_BeginScissorMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("BeginScissorMode expects x, y, width, height");
            return 0;
        }
        BeginScissorMode(args[0].asNumber(), args[1].asNumber(),
                         args[2].asNumber(), args[3].asNumber());
        return 0;
    }

    static int native_EndScissorMode(Interpreter *vm, int argc, Value *args)
    {
        EndScissorMode();
        return 0;
    }

    // ========================================
    // BLEND MODE
    // ========================================

    static int native_BeginBlendMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("BeginBlendMode expects mode");
            return 0;
        }
        BeginBlendMode(args[0].asNumber());
        return 0;
    }

    static int native_EndBlendMode(Interpreter *vm, int argc, Value *args)
    {
        EndBlendMode();
        return 0;
    } 

    // ========================================
    // COLOR HELPERS
    // ========================================

    static int native_Fade(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isNativeStructInstance())
        {
            Error("Fade expects Color and alpha");
            return 0;
        }
        auto *colorInst = args[0].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;
        double alpha = args[1].asNumber();
        Color result = Fade(*color, alpha);

        // Modify original color
        color->r = result.r;
        color->g = result.g;
        color->b = result.b;
        color->a = result.a;

        return 0;
    }

    static  const char* formatBytes(size_t bytes)
{
    static char buffer[32];

    if (bytes < 1024)
        snprintf(buffer, sizeof(buffer), "%zu B", bytes);
    else if (bytes < 1024 * 1024)
        snprintf(buffer, sizeof(buffer), "%zu KB", bytes / 1024);
    else
        snprintf(buffer, sizeof(buffer), "%zu MB", bytes / (1024 * 1024));

    return buffer;
}

    static int native_DrawStats(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawStats expects 2 arguments");
            return 0;
        }
        int x = args[0].asInt();
        int y = args[1].asInt();

        
        DrawRectangle(x-2, y-2, 300, 104, Fade(BLACK, 0.5f));
        
        DrawRectangleLines(x-2, y-2, 300, 104, WHITE);
        DrawText(TextFormat("RAM: %s", formatBytes(vm->getTotalAlocated())), x, y + 16, 20, WHITE);
        DrawText(TextFormat("Classes %d, Structs %d", vm->getTotalClasses(), vm->getTotalStructs()), x, y + 32, 20, WHITE);
        DrawText(TextFormat("Arrays %d Maps  %d", vm->getTotalArrays(), vm->getTotalMaps()), x, y + 48, 20, WHITE);
        DrawText(TextFormat("Native Classes %d, Structs %d", vm->getTotalNativeClasses(), vm->getTotalNativeStructs()), x, y + 64, 20, WHITE);


        return 0;
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
           .addFunction("DrawStats", native_DrawStats, 2)

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



        // Color
           .addFunction("Fade", native_Fade, 2)

        // Blend mode constants
           .addByte("BLEND_ALPHA", BLEND_ALPHA)
           .addByte("BLEND_ADDITIVE", BLEND_ADDITIVE)
           .addByte("BLEND_MULTIPLIED", BLEND_MULTIPLIED)
           .addByte("BLEND_ADD_COLORS", BLEND_ADD_COLORS)
           .addByte("BLEND_SUBTRACT_COLORS", BLEND_SUBTRACT_COLORS)
           .addByte("BLEND_ALPHA_PREMULTIPLY", BLEND_ALPHA_PREMULTIPLY);
    }

}
