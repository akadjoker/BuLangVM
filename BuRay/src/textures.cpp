#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // LOAD/UNLOAD
    // ========================================

    static Value native_LoadTexture(Interpreter *vm, int argc, Value *args)
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

    static Value native_UnloadTexture(Interpreter *vm, int argc, Value *args)
    {
        Texture2D *tex = (Texture2D *)args[0].asPointer();

        UnloadTexture(*tex);
        delete tex;

        return vm->makeNil();
    }

    // ========================================
    // DRAW TEXTURE
    // ========================================

    static Value native_DrawTexture(Interpreter *vm, int argc, Value *args)
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
        int x = args[1].asNumber();
        int y = args[2].asNumber();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTexture(*tex, x, y, *tint);
        return vm->makeNil();
    }

    static Value native_DrawTextureV(Interpreter *vm, int argc, Value *args)
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

    static Value native_DrawTextureEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawTextureEx expects 6 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("DrawTextureEx expects Texture2D");
            return vm->makeNil();
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawTextureEx expects Color");
            return vm->makeNil();
        }
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int x = (int)args[1].asNumber();
        int y = (int)args[2].asNumber();
        double rotation = args[3].asNumber();
        double scale = args[4].asNumber();
        Vector2 position = {(float)x, (float)y};

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextureEx(*tex, position, rotation, scale, *tint);
        return vm->makeNil();
    }

    // ========================================
    // REGISTER TEXTURES
    // ========================================

    void register_textures(ModuleBuilder &mod)
    {
        mod.addFunction("LoadTexture", native_LoadTexture, 1)
           .addFunction("UnloadTexture", native_UnloadTexture, 1)
           .addFunction("DrawTexture", native_DrawTexture, 4)
           .addFunction("DrawTextureV", native_DrawTextureV, 3)
           .addFunction("DrawTextureEx", native_DrawTextureEx, 6);
    }

}
