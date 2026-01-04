#include "bidings.hpp"

#include <cmath>

namespace RaylibBindings
{

    
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

  


    // ========================================
    // VECTOR2
    // ========================================

    void registerVector2(Interpreter &vm)
    {
        auto *vec2 = vm.registerNativeStruct(
            "Vector2",
            sizeof(Vector2),
            nullptr, // Sem constructor
            nullptr  // Sem destructor
     
        );

        vm.addStructField(vec2, "x", offsetof(Vector2, x), FieldType::FLOAT);
        vm.addStructField(vec2, "y", offsetof(Vector2, y), FieldType::FLOAT);
    }

    // ========================================
    // VECTOR3
    // ========================================

    void registerVector3(Interpreter &vm)
    {
        auto *vec3 = vm.registerNativeStruct(
            "Vector3",
            sizeof(Vector3),
            nullptr,
            nullptr);

        vm.addStructField(vec3, "x", offsetof(Vector3, x), FieldType::FLOAT);
        vm.addStructField(vec3, "y", offsetof(Vector3, y), FieldType::FLOAT);
        vm.addStructField(vec3, "z", offsetof(Vector3, z), FieldType::FLOAT);
    }

    // ========================================
    // RECTANGLE
    // ========================================

    void rectangle_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
    {
        Rectangle *v = (Rectangle *)buffer;
        v->x = 0;
        v->y = 0;
        v->width = 0;
        v->height = 0;
    }

    void registerRectangle(Interpreter &vm)
    {
        auto *rect = vm.registerNativeStruct(
            "Rectangle",
            sizeof(Rectangle),
            rectangle_ctor,
            nullptr);

        vm.addStructField(rect, "x", offsetof(Rectangle, x), FieldType::FLOAT);
        vm.addStructField(rect, "y", offsetof(Rectangle, y), FieldType::FLOAT);
        vm.addStructField(rect, "width", offsetof(Rectangle, width), FieldType::FLOAT);
        vm.addStructField(rect, "height", offsetof(Rectangle, height), FieldType::FLOAT);
    }

    // ========================================
    // COLOR
    // ========================================

    void color_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
    {
        Color *v = (Color *)buffer;
        v->r = args[0].asByte();
        v->g = args[1].asByte();
        v->b = args[2].asByte();
        v->a = args[3].asByte();
        // Info("Color(%d, %d, %d, %d)", v->r, v->g, v->b, v->a);
    }

    // void color_dtor(Interpreter *vm, void *buffer)
    // {
    //     Color *v = (Color *)buffer;
    //     //Info("Color(%d, %d, %d, %d)", v->r, v->g, v->b, v->a);
    // }

    void registerColor(Interpreter &vm)
    {
        auto *color = vm.registerNativeStruct(
            "Color",
            sizeof(Color),
            color_ctor,
            nullptr);

        vm.addStructField(color, "r", offsetof(Color, r), FieldType::BYTE);
        vm.addStructField(color, "g", offsetof(Color, g), FieldType::BYTE);
        vm.addStructField(color, "b", offsetof(Color, b), FieldType::BYTE);
        vm.addStructField(color, "a", offsetof(Color, a), FieldType::BYTE);
    }

    // ========================================
    // CAMERA2D
    // ========================================

    void registerCamera2D(Interpreter &vm)
    {
        auto *camera = vm.registerNativeStruct(
            "Camera2D",
            sizeof(Camera2D),
            nullptr,
            nullptr
     );

        vm.addStructField(camera, "offset", offsetof(Camera2D, offset), FieldType::POINTER); // Vector2
        vm.addStructField(camera, "target", offsetof(Camera2D, target), FieldType::POINTER); // Vector2
        vm.addStructField(camera, "rotation", offsetof(Camera2D, rotation), FieldType::FLOAT);
        vm.addStructField(camera, "zoom", offsetof(Camera2D, zoom), FieldType::FLOAT);
    }

    // ========================================
    // WINDOW
    // ========================================

    Value native_InitWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            return vm->makeNil();
        }
        int width = args[0].asInt();
        int height = args[1].asInt();
        const char *title = args[2].asString()->chars();

        InitWindow(width, height, title);

        return vm->makeNil();
    }

    Value native_CloseWindow(Interpreter *vm, int argc, Value *args)
    {
        CloseWindow();
        return vm->makeNil();
    }

    Value native_WindowShouldClose(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(WindowShouldClose());
    }

    Value native_SetTargetFPS(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SetTargetFPS expects 1 argument");
            return vm->makeNil();
        }
        SetTargetFPS(args[0].asInt());
        return vm->makeNil();
    }

    Value native_GetFPS(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetFPS());
    }

    Value native_GetFrameTime(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeDouble(GetFrameTime());
    }

    // ========================================
    // DRAWING
    // ========================================

    Value native_DrawText(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawText expects 5 arguments");
            return vm->makeNil();
        }

        if (!args[0].isString())
        {
            Error("DrawText expects argument 0 to be string");
            return vm->makeNil();
        }

        if (!args[4].isNativeStructInstance())
        {
            Error("DrawText expects argument 4 to be Color");
            return vm->makeNil();
        }

        auto *colorInst = args[4].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        const char *text = args[0].asString()->chars();
        int x = args[1].asInt();
        int y = args[2].asInt();
        int fontSize = args[3].asInt();

        DrawText(text, x, y, fontSize, *tint); 
        return vm->makeNil();
    }

    Value native_BeginDrawing(Interpreter *vm, int argc, Value *args)
    {
        BeginDrawing();
        return vm->makeNil();
    }

    Value native_EndDrawing(Interpreter *vm, int argc, Value *args)
    {
        EndDrawing();
        return vm->makeNil();
    }

    Value native_ClearBackground(Interpreter *vm, int argc, Value *args)
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

    Value native_DrawPixel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawPixel expects 3 arguments");
            return vm->makeNil();
        }

        if (!args[2].isNativeStructInstance())
        {
            Error("DrawPixel expects Color");
            return vm->makeNil();
        }

        int x =  args[0].asInt();
        int y = args[1].asInt();

        auto *inst = args[2].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawPixel(x, y, *color);
        return vm->makeNil();
    }

    Value native_DrawLine(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawLine expects 5 arguments");
            return vm->makeNil();
        }

        if (!args[4].isNativeStructInstance())
        {
            Error("DrawLine expects Color");
            return vm->makeNil();
        }

        int x1 =  args[0].asInt();
        int y1 =  args[1].asInt();
        int x2 =  args[2].asInt();
        int y2 =  args[3].asInt();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawLine(x1, y1, x2, y2, *color);
        return vm->makeNil();
    }

    Value native_DrawCircle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawCircle expects 4 arguments");
            return vm->makeNil();
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawCircle expects Color");
            return vm->makeNil();
        }
        int x = args[0].asInt();
        int y = args[1].asInt();
        float radius = args[2].asDouble();

        auto *inst = args[3].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawCircle(x, y, radius, *color);
        return vm->makeNil();
    }

    Value native_DrawCircleV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawCircleV expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawCircleV expects Color");
            return vm->makeNil();
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;

        float radius = args[1].asDouble();

        auto *colorInst = args[2].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCircleV(*pos, radius, *color);
        return vm->makeNil();
    }

    Value native_DrawRectangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawRectangle expects 5 arguments");
            return vm->makeNil();
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawRectangle expects Color");
            return vm->makeNil();
        }
        int x = args[0].asInt();
        int y = args[1].asInt();
        int width = args[2].asInt();
        int height = args[3].asInt();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawRectangle(x, y, width, height, *color);
        return vm->makeNil();
    }

    Value native_DrawRectangleRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawRectangleRec expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isNativeStructInstance())
        {
            Error("DrawRectangleRec expects Rectangle");
            return vm->makeNil();
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawRectangleRec expects Color");
            return vm->makeNil();
        }
        auto *rectInst = args[0].asNativeStructInstance();
        Rectangle *rect = (Rectangle *)rectInst->data;

        auto *colorInst = args[1].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawRectangleRec(*rect, *color);
        return vm->makeNil();
    }

    // ========================================
    // TEXTURES
    // ========================================

    Value native_LoadTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("LoadTexture expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isString())
        {
            Error("LoadTexture expects string");
            return vm->makeNil();
        }
        const char *filename = args[0].asStringChars();

        Texture2D tex = LoadTexture(filename);
        Texture2D *texPtr = new Texture2D(tex);

        return vm->makePointer(texPtr);
    }

    Value native_UnloadTexture(Interpreter *vm, int argc, Value *args)
    {
        Texture2D *tex = (Texture2D *)args[0].asPointer();

        UnloadTexture(*tex);
        delete tex;

        return vm->makeNil();
    }

    Value native_DrawTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawTexture expects 4 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("DrawTexture expects Texture2D");
            return vm->makeNil();
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTexture expects Color");
            return vm->makeNil();
        }
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int x = args[1].asInt();
        int y = args[2].asInt();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTexture(*tex, x, y, *tint);
        return vm->makeNil();
    }

    Value native_DrawTextureV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawTextureV expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("DrawTextureV expects Texture2D");
            return vm->makeNil();
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawTextureV expects Vector2");
            return vm->makeNil();
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawTextureV expects Color");
            return vm->makeNil();
        }

        Texture2D *tex = (Texture2D *)args[0].asPointer();

        auto *posInst = args[1].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;

        auto *colorInst = args[2].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextureV(*tex, *pos, *tint);
        return vm->makeNil();
    }

    Value native_DrawFps(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawFps expects 2 arguments");
            return vm->makeNil();
        }
        int x = args[0].asInt();
        int y = args[1].asInt();

        
      //  DrawRectangle(0, 0, 300, 114, Fade(BLACK, 0.5f));
        DrawFPS(x, y);
        //DrawRectangleLines(x, y, 200, 64, WHITE);
        // DrawText(TextFormat("RAM: %s", formatBytes(vm->getTotalAlocated())), 10, y + 16, 20, WHITE);
        // DrawText(TextFormat("Classes %d, Structs %d", vm->getTotalClasses(), vm->getTotalStructs()), 10, y + 32, 20, WHITE);
        // DrawText(TextFormat("Arrays %d Maps  %d", vm->getTotalArrays(), vm->getTotalMaps()), 10, y + 48, 20, WHITE);
        // DrawText(TextFormat("Native Classes %d, Structs %d", vm->getTotalNativeClasses(), vm->getTotalNativeStructs()), 10, y + 64, 20, WHITE);


        return vm->makeNil();
    }

    Value native_IsMouseButtonDown(Interpreter *vm, int argc, Value *args)
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

        return vm->makeBool(IsMouseButtonDown(args[0].asInt()));
    }

    Value native_IsMouseButtonPressed(Interpreter *vm, int argc, Value *args)
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

        return vm->makeBool(IsMouseButtonPressed(args[0].asInt()));
    }

    Value native_IsMouseButtonReleased(Interpreter *vm, int argc, Value *args)
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

        return vm->makeBool(IsMouseButtonReleased(args[0].asInt()));
    }

    Value native_IsMouseButtonUp(Interpreter *vm, int argc, Value *args)
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

        return vm->makeBool(IsMouseButtonUp(args[0].asInt()));
    }

    Value native_GetMouseX(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetMouseX());
    }

    Value native_GetMouseY(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeInt(GetMouseY());
    }

    Value native_GetMousePosition(Interpreter *vm, int argc, Value *args)
    {
        Vector2 v = GetMousePosition();
        return vm->makeInt(-1);
    }

}