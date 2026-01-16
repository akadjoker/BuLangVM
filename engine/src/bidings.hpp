#pragma once

#include "interpreter.hpp"
#include <raylib.h>

namespace RaylibBindings
{ 

    // ========================================
    // NATIVE STRUCTS
    // ========================================

    void registerVector2(Interpreter &vm);
    void registerVector3(Interpreter &vm);
    void registerRectangle(Interpreter &vm);
    void registerColor(Interpreter &vm);
    void registerCamera2D(Interpreter &vm);

    // ========================================
    // WINDOW
    // ========================================

    Value native_InitWindow(Interpreter *vm, int argc, Value *args);
    Value native_CloseWindow(Interpreter *vm, int argc, Value *args);
    Value native_WindowShouldClose(Interpreter *vm, int argc, Value *args);
    Value native_SetTargetFPS(Interpreter *vm, int argc, Value *args);
    Value native_GetFPS(Interpreter *vm, int argc, Value *args);
    Value native_GetFrameTime(Interpreter *vm, int argc, Value *args);

    // ========================================
    // DRAWING
    // ========================================

    Value native_BeginDrawing(Interpreter *vm, int argc, Value *args);
    Value native_EndDrawing(Interpreter *vm, int argc, Value *args);
    Value native_ClearBackground(Interpreter *vm, int argc, Value *args);

    Value native_DrawPixel(Interpreter *vm, int argc, Value *args);
    Value native_DrawLine(Interpreter *vm, int argc, Value *args);
    Value native_DrawCircle(Interpreter *vm, int argc, Value *args);
    Value native_DrawCircleV(Interpreter *vm, int argc, Value *args);
    Value native_DrawRectangle(Interpreter *vm, int argc, Value *args);
    Value native_DrawRectangleRec(Interpreter *vm, int argc, Value *args);

    // ========================================
    // TEXTURES
    // ========================================

    Value native_LoadTexture(Interpreter *vm, int argc, Value *args);
    Value native_UnloadTexture(Interpreter *vm, int argc, Value *args);
    Value native_DrawTexture(Interpreter *vm, int argc, Value *args);
    Value native_DrawTextureV(Interpreter *vm, int argc, Value *args);
    Value native_DrawTextureEx(Interpreter *vm, int argc, Value *args);

    // ========================================
    // TEXT
    // ========================================

    Value native_DrawText(Interpreter *vm, int argc, Value *args);
    Value native_DrawFps(Interpreter *vm, int argc, Value *args);
    Value native_MeasureText(Interpreter *vm, int argc, Value *args);
    Value native_LoadFont(Interpreter *vm, int argc, Value *args);
    Value native_UnloadFont(Interpreter *vm, int argc, Value *args);
    Value native_DrawTextEx(Interpreter *vm, int argc, Value *args);

    // ========================================
    // INPUT - KEYBOARD
    // ========================================

    Value native_IsKeyPressed(Interpreter *vm, int argc, Value *args);
    Value native_IsKeyDown(Interpreter *vm, int argc, Value *args);
    Value native_IsKeyReleased(Interpreter *vm, int argc, Value *args);
    Value native_IsKeyUp(Interpreter *vm, int argc, Value *args);
    Value native_GetKeyPressed(Interpreter *vm, int argc, Value *args);

    // ========================================
    // INPUT - MOUSE
    // ========================================

    Value native_IsMouseButtonPressed(Interpreter *vm, int argc, Value *args);
    Value native_IsMouseButtonDown(Interpreter *vm, int argc, Value *args);
    Value native_IsMouseButtonReleased(Interpreter *vm, int argc, Value *args);
    Value native_GetMousePosition(Interpreter *vm, int argc, Value *args);
    Value native_GetMouseX(Interpreter *vm, int argc, Value *args);
    Value native_GetMouseY(Interpreter *vm, int argc, Value *args);

    // ========================================
    // AUDIO
    // ========================================

    Value native_InitAudioDevice(Interpreter *vm, int argc, Value *args);
    Value native_CloseAudioDevice(Interpreter *vm, int argc, Value *args);
    Value native_LoadSound(Interpreter *vm, int argc, Value *args);
    Value native_UnloadSound(Interpreter *vm, int argc, Value *args);
    Value native_PlaySound(Interpreter *vm, int argc, Value *args);
    Value native_StopSound(Interpreter *vm, int argc, Value *args);

    Value native_LoadMusicStream(Interpreter *vm, int argc, Value *args);
    Value native_UnloadMusicStream(Interpreter *vm, int argc, Value *args);
    Value native_PlayMusicStream(Interpreter *vm, int argc, Value *args);
    Value native_UpdateMusicStream(Interpreter *vm, int argc, Value *args);
    Value native_StopMusicStream(Interpreter *vm, int argc, Value *args);

    // ========================================
    // TIMING
    // ========================================

    Value native_GetTime(Interpreter *vm, int argc, Value *args);

    // ========================================
    // MATH
    // ========================================

    Value native_Clamp(Interpreter *vm, int argc, Value *args);
    Value native_Lerp(Interpreter *vm, int argc, Value *args);

    // ========================================
    // REGISTRATION
    // ========================================

    void registerAll(Interpreter &vm);

} // namespace RaylibBindings