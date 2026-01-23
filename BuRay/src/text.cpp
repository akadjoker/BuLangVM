#include "bidings.hpp"

namespace RaylibBindings
{

    // ========================================
    // DRAW TEXT (DEFAULT FONT)
    // ========================================

    static Value native_DrawText(Interpreter *vm, int argc, Value *args)
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
        int x = args[1].asNumber();
        int y = args[2].asNumber();
        int fontSize = args[3].asNumber();

        DrawText(text, x, y, fontSize, *tint);
        return vm->makeNil();
    }

    static Value native_DrawFPS(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawFPS expects 2 arguments");
            return vm->makeNil();
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();

        DrawFPS(x, y);
        return vm->makeNil();
    }

    static Value native_MeasureText(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("MeasureText expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isString())
        {
            Error("MeasureText expects string");
            return vm->makeNil();
        }

        const char *text = args[0].asStringChars();
        int fontSize = args[1].asNumber();

        return vm->makeInt(MeasureText(text, fontSize));
    }

    // ========================================
    // FONT LOADING
    // ========================================

    static Value native_LoadFont(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
        {
            Error("LoadFont expects filename");
            return vm->makeNil();
        }
        const char *filename = args[0].asStringChars();
        Font font = LoadFont(filename);
        Font *ptr = new Font(font);
        return vm->makePointer(ptr);
    }

    static Value native_LoadFontEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2 || !args[0].isString())
        {
            Error("LoadFontEx expects filename and fontSize");
            return vm->makeNil();
        }
        const char *filename = args[0].asStringChars();
        int fontSize = args[1].asNumber();
        Font font = LoadFontEx(filename, fontSize, nullptr, 0);
        Font *ptr = new Font(font);
        return vm->makePointer(ptr);
    }

    static Value native_UnloadFont(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Font *font = (Font *)args[0].asPointer();
        UnloadFont(*font);
        delete font;
        return vm->makeNil();
    }

    static Value native_IsFontReady(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeBool(false);
        Font *font = (Font *)args[0].asPointer();
        return vm->makeBool(IsFontReady(*font));
    }

    // ========================================
    // DRAW TEXT WITH FONT
    // ========================================

    static Value native_DrawTextEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawTextEx expects 7 arguments (font, text, x, y, fontSize, spacing, color)");
            return vm->makeNil();
        }

        if (!args[0].isPointer())
        {
            Error("DrawTextEx expects Font");
            return vm->makeNil();
        }
        if (!args[1].isString())
        {
            Error("DrawTextEx expects string");
            return vm->makeNil();
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawTextEx expects Color");
            return vm->makeNil();
        }

        Font *font = (Font *)args[0].asPointer();
        const char *text = args[1].asStringChars();
        float x = args[2].asDouble();
        float y = args[3].asDouble();
        float fontSize = args[4].asDouble();
        float spacing = args[5].asDouble();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextEx(*font, text, {x, y}, fontSize, spacing, *tint);
        return vm->makeNil();
    }

    static Value native_DrawTextPro(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 10)
        {
            Error("DrawTextPro expects 10 arguments (font, text, x, y, originX, originY, rotation, fontSize, spacing, color)");
            return vm->makeNil();
        }

        if (!args[0].isPointer() || !args[1].isString() || !args[9].isNativeStructInstance())
        {
            Error("DrawTextPro expects Font, string, and Color");
            return vm->makeNil();
        }

        Font *font = (Font *)args[0].asPointer();
        const char *text = args[1].asStringChars();
        float x = args[2].asDouble();
        float y = args[3].asDouble();
        float originX = args[4].asDouble();
        float originY = args[5].asDouble();
        float rotation = args[6].asDouble();
        float fontSize = args[7].asDouble();
        float spacing = args[8].asDouble();

        auto *colorInst = args[9].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawTextPro(*font, text, {x, y}, {originX, originY}, rotation, fontSize, spacing, *tint);
        return vm->makeNil();
    }

    static Value native_MeasureTextEx(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("MeasureTextEx expects 4 arguments (font, text, fontSize, spacing)");
            return vm->makeNil();
        }

        if (!args[0].isPointer() || !args[1].isString())
        {
            Error("MeasureTextEx expects Font and string");
            return vm->makeNil();
        }

        Font *font = (Font *)args[0].asPointer();
        const char *text = args[1].asStringChars();
        float fontSize = args[2].asDouble();
        float spacing = args[3].asDouble();

        Vector2 size = MeasureTextEx(*font, text, fontSize, spacing);
        // Return width as int for now (TODO: return Vector2)
        return vm->makeInt((int)size.x);
    }

    // ========================================
    // GET DEFAULT FONT
    // ========================================

    static Value native_GetFontDefault(Interpreter *vm, int argc, Value *args)
    {
        Font font = GetFontDefault();
        Font *ptr = new Font(font);
        return vm->makePointer(ptr);
    }

    // ========================================
    // REGISTER TEXT
    // ========================================

    void register_text(ModuleBuilder &mod)
    {
        // Default font text
        mod.addFunction("DrawText", native_DrawText, 5)
           .addFunction("DrawFPS", native_DrawFPS, 2)
           .addFunction("MeasureText", native_MeasureText, 2)

        // Font loading
           .addFunction("LoadFont", native_LoadFont, 1)
           .addFunction("LoadFontEx", native_LoadFontEx, 2)
           .addFunction("UnloadFont", native_UnloadFont, 1)
           .addFunction("IsFontReady", native_IsFontReady, 1)
           .addFunction("GetFontDefault", native_GetFontDefault, 0)

        // Font drawing
           .addFunction("DrawTextEx", native_DrawTextEx, 7)
           .addFunction("DrawTextPro", native_DrawTextPro, 10)
           .addFunction("MeasureTextEx", native_MeasureTextEx, 4);
    }

}
