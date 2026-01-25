#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // LOAD/UNLOAD
    // ========================================

    static int native_LoadTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("LoadTexture expects 1 argument");
            return 0;
        }
        if (!args[0].isString())
        {
            Error("LoadTexture expects string");
            return 0;
        }
        const char *filename = args[0].asStringChars();

        Texture2D tex = LoadTexture(filename);
        Texture2D *texPtr = new Texture2D(tex);

        vm->push(vm->makePointer(texPtr));

        return 1;
    }

    static int native_UnloadTexture(Interpreter *vm, int argc, Value *args)
    {
        Texture2D *tex = (Texture2D *)args[0].asPointer();

        UnloadTexture(*tex);
        delete tex;

        return 0;
    }

    // ========================================
    // DRAW TEXTURE
    // ========================================

    static int native_DrawTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawTexture expects 4 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTexture expects Texture2D");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTexture expects Color");
            return 0;
        }
        Texture2D *tex = (Texture2D *)args[0].asPointer();
        int x = args[1].asNumber();
        int y = args[2].asNumber();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTexture(*tex, x, y, *tint);
        return 0;
    }

    static int native_DrawTextureV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawTextureV expects 3 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTextureV expects Texture2D");
            return 0;
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawTextureV expects Vector2");
            return 0;
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawTextureV expects Color");
            return 0;
        }

        Texture2D *tex = (Texture2D *)args[0].asPointer();

        auto *posInst = args[1].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;

        auto *colorInst = args[2].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextureV(*tex, *pos, *tint);
        return 0;
    }

    static int native_DrawTextureEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawTextureEx expects 6 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTextureEx expects Texture2D");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawTextureEx expects Color");
            return 0;
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
        return 0;
    }

    static  int native_DrawTexturePro(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawTexturePro expects 5 arguments");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawTexturePro expects Texture2D");
            return 0;
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Rectangle");
            return 0;
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Rectangle");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Vector2");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawTexturePro expects Color");
            return 0;
        }

        Texture2D *tex = (Texture2D *)args[0].asPointer();

        auto *sourceInst = args[1].asNativeStructInstance();
        Rectangle *source = (Rectangle *)sourceInst->data;
 

        auto *destInst = args[2].asNativeStructInstance();
        Rectangle *dest = (Rectangle *)destInst->data;

        auto *originInst = args[3].asNativeStructInstance();
        Vector2 *origin = (Vector2 *)originInst->data;


        double rotation = args[4].asNumber();
        

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTexturePro(*tex, *source, *dest, *origin, rotation, *tint);
        return 0;
        
    }

    // ========================================
    // RENDER TEXTURES
    // ========================================

    static int native_LoadRenderTexture(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("LoadRenderTexture expects width, height");
            return 0;
        }

        int w = (int)args[0].asNumber();
        int h = (int)args[1].asNumber();

        // IMPORTANTE: Guardamos a RenderTexture2D completa, não só a textura!
        RenderTexture2D rt = LoadRenderTexture(w, h);
        RenderTexture2D *rtPtr = new RenderTexture2D(rt);

        vm->push(vm->makePointer(rtPtr));
        return 1;
    }

    static int native_UnloadRenderTexture(Interpreter *vm, int argc, Value *args)
    {
        if (!args[0].isPointer())
            return 0;
        RenderTexture2D *rt = (RenderTexture2D *)args[0].asPointer();
        UnloadRenderTexture(*rt);
        delete rt;
        return 0;
    }

    static int native_GetRenderTextureTexture(Interpreter *vm, int argc, Value *args)
    {
        RenderTexture2D *rt = (RenderTexture2D *)args[0].asPointer();
        vm->push(vm->makePointer(&(rt->texture)));
        return 1;
    }

    static int native_BeginTextureMode(Interpreter *vm, int argc, Value *args)
    {
        if (!args[0].isPointer())
        {
            vm->runtimeError("BeginTextureMode expects RenderTexture2D");
            return 0;
        }

        RenderTexture2D *rt = (RenderTexture2D *)args[0].asPointer();
        BeginTextureMode(*rt);
        return 0;
    }

    static int native_EndTextureMode(Interpreter *vm, int argc, Value *args)
    {
        EndTextureMode();
        return 0;
    }

    // LoadTextureFromImage

    // ========================================
    // REGISTER TEXTURES
    // ========================================

    void register_textures(ModuleBuilder &mod)
    {
        mod.addFunction("LoadTexture", native_LoadTexture, 1)
            .addFunction("UnloadTexture", native_UnloadTexture, 1)
            .addFunction("DrawTexture", native_DrawTexture, 4)
            .addFunction("DrawTexturePro", native_DrawTexturePro, 6)
            .addFunction("DrawTextureV", native_DrawTextureV, 3)
            .addFunction("DrawTextureEx", native_DrawTextureEx, 6)
            .addFunction("LoadRenderTexture", native_LoadRenderTexture, 2)
            .addFunction("UnloadRenderTexture", native_UnloadRenderTexture, 1)
            .addFunction("BeginTextureMode", native_BeginTextureMode, 1)
            .addFunction("EndTextureMode", native_EndTextureMode, 0)
            .addFunction("GetRenderTextureTexture", native_GetRenderTextureTexture, 1);
    }

}
