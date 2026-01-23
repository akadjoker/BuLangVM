#include "bidings.hpp"

namespace RaylibBindings
{

    // ========================================
    // PIXEL
    // ========================================

    static Value native_DrawPixel(Interpreter *vm, int argc, Value *args)
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

        int x = args[0].asNumber();
        int y = args[1].asNumber();

        auto *inst = args[2].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawPixel(x, y, *color);
        return vm->makeNil();
    }

    // ========================================
    // LINE
    // ========================================

    static Value native_DrawLine(Interpreter *vm, int argc, Value *args)
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

        int x1 = args[0].asNumber();
        int y1 = args[1].asNumber();
        int x2 = args[2].asNumber();
        int y2 = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawLine(x1, y1, x2, y2, *color);
        return vm->makeNil();
    }

    // ========================================
    // CIRCLE
    // ========================================

    static Value native_DrawCircle(Interpreter *vm, int argc, Value *args)
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
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        float radius = args[2].asDouble();

        auto *inst = args[3].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawCircle(x, y, radius, *color);
        return vm->makeNil();
    }

    static Value native_DrawCircleV(Interpreter *vm, int argc, Value *args)
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

    // ========================================
    // RECTANGLE
    // ========================================

    static Value native_DrawRectangle(Interpreter *vm, int argc, Value *args)
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
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawRectangle(x, y, width, height, *color);
        return vm->makeNil();
    }

    static Value native_DrawRectangleRec(Interpreter *vm, int argc, Value *args)
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
    // REGISTER SHAPES
    // ========================================

    void register_shapes(ModuleBuilder &mod)
    {
        mod.addFunction("DrawPixel", native_DrawPixel, 3)
           .addFunction("DrawLine", native_DrawLine, 5)
           .addFunction("DrawCircle", native_DrawCircle, 4)
           .addFunction("DrawCircleV", native_DrawCircleV, 3)
           .addFunction("DrawRectangle", native_DrawRectangle, 5)
           .addFunction("DrawRectangleRec", native_DrawRectangleRec, 2);
    }

}
