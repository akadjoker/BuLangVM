#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // PIXEL
    // ========================================

    static int native_DrawPixel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawPixel expects 3 arguments");
            return 0;
        }

        if (!args[2].isNativeStructInstance())
        {
            Error("DrawPixel expects Color");
            return 0;
        }

        int x = args[0].asNumber();
        int y = args[1].asNumber();

        auto *inst = args[2].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawPixel(x, y, *color);
        return 0;
    }

    // ========================================
    // LINE
    // ========================================

    static int native_DrawLine(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawLine expects 5 arguments");
            return 0;
        }

        if (!args[4].isNativeStructInstance())
        {
            Error("DrawLine expects Color");
            return 0;
        }

        int x1 = args[0].asNumber();
        int y1 = args[1].asNumber();
        int x2 = args[2].asNumber();
        int y2 = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawLine(x1, y1, x2, y2, *color);
        return 0;
    }

    // ========================================
    // CIRCLE
    // ========================================

    static int native_DrawCircle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawCircle expects 4 arguments");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawCircle expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        float radius = args[2].asDouble();

        auto *inst = args[3].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawCircle(x, y, radius, *color);
        return 0;
    }

    static int native_DrawCircleLines(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawCircleLines expects 4 arguments");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawCircleLines expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        float radius = args[2].asDouble();

        auto *inst = args[3].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawCircleLines(x, y, radius, *color);
        return 0;
    }

    static int native_DrawCircleV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("DrawCircleV expects 3 arguments");
            return 0;
        }
        if (!args[2].isNativeStructInstance())
        {
            Error("DrawCircleV expects Color");
            return 0;
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;

        float radius = args[1].asDouble();

        auto *colorInst = args[2].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCircleV(*pos, radius, *color);
        return 0;
    }

    // ========================================
    // RECTANGLE
    // ========================================

    static int native_DrawRectangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawRectangle expects 5 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawRectangle expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawRectangle(x, y, width, height, *color);
        return 0;
    }

    static int native_DrawRectangleRec(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawRectangleRec expects 2 arguments");
            return 0;
        }
        if (!args[0].isNativeStructInstance())
        {
            Error("DrawRectangleRec expects Rectangle");
            return 0;
        }
        if (!args[1].isNativeStructInstance())
        {
            Error("DrawRectangleRec expects Color");
            return 0;
        }
        auto *rectInst = args[0].asNativeStructInstance();
        Rectangle *rect = (Rectangle *)rectInst->data;

        auto *colorInst = args[1].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawRectangleRec(*rect, *color);
        return 0;
    }

    static int native_DrawRectangleLines(Interpreter *vm, int argc, Value *args)    
    {
        if (argc != 5)
        {
            Error("DrawRectangleLines expects 5 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawRectangleLines expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        DrawRectangleLines(x, y, width, height, *color);
        return 0;
    }

    //DrawRectangleGradientV

    static int native_DrawRectangleGradientV(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawRectangleGradientV expects 6 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance() || !args[5].isNativeStructInstance())
        {
            Error("DrawRectangleGradientV expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        auto *color2Inst = args[5].asNativeStructInstance();
        Color *color2 = (Color *)color2Inst->data;

        DrawRectangleGradientV(x, y,  width, height, *color, *color2);
        return 0;
    }

    static int native_DrawRectangleGradientH(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawRectangleGradientH expects 5 arguments");
            return 0;
        }
        if (!args[4].isNativeStructInstance() || !args[5].isNativeStructInstance())
        {
            Error("DrawRectangleGradientH expects Color");
            return 0;
        }
        int x = args[0].asNumber();
        int y = args[1].asNumber();
        int width = args[2].asNumber();
        int height = args[3].asNumber();

        auto *inst = args[4].asNativeStructInstance();
        Color *color = (Color *)inst->data;

        auto *color2Inst = args[5].asNativeStructInstance();
        Color *color2 = (Color *)color2Inst->data;

        DrawRectangleGradientH(x, y,  width, height, *color, *color2);
        return 0;
    }

    //TRIANGLE

    static int native_DrawTriangle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawTriangle expects 4 arguments");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawTriangle expects Color");
            return 0;
        }
        auto *v1Inst = args[0].asNativeStructInstance();
        Vector2 *v1 = (Vector2 *)v1Inst->data;

      

         auto *v2Inst = args[1].asNativeStructInstance();
         Vector2 *v2 = (Vector2 *)v2Inst->data;

  

         auto *v3Inst = args[2].asNativeStructInstance();
         Vector2 *v3 = (Vector2 *)v3Inst->data;

      

         auto *colorInst = args[3].asNativeStructInstance();
         Color *color = (Color *)colorInst->data;

    

         DrawTriangle(*v1, *v2, *v3, *color);
        return 0;
    }

    // ========================================
    // REGISTER SHAPES
    // ========================================

    void register_shapes(ModuleBuilder &mod)
    {
        mod.addFunction("DrawPixel", native_DrawPixel, 3)
           .addFunction("DrawTriangle", native_DrawTriangle, 4)

           .addFunction("DrawLine", native_DrawLine, 5)
           .addFunction("DrawCircle", native_DrawCircle, 4)
           .addFunction("DrawCircleV", native_DrawCircleV, 3)
           .addFunction("DrawCircleLines", native_DrawCircleLines, 4)
           .addFunction("DrawRectangle", native_DrawRectangle, 5)
           .addFunction("DrawRectangleLines", native_DrawRectangleLines, 5)
           .addFunction("DrawRectangleGradientV", native_DrawRectangleGradientV, 6)
           .addFunction("DrawRectangleGradientH", native_DrawRectangleGradientH, 6)
           .addFunction("DrawRectangleRec", native_DrawRectangleRec, 2);

    }

}
