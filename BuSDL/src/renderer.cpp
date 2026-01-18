#include "bidings.hpp"

namespace SDLBindings
{
    Value native_SDL_CreateRenderer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("SDL_CreateRenderer expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_CreateRenderer expects window pointer");
            return vm->makeNil();
        }

        SDL_Window *window = (SDL_Window *)args[0].asPointer();
        int index = args[1].asNumber();
        Uint32 flags = args[2].asNumber();

        SDL_Renderer *renderer = SDL_CreateRenderer(window, index, flags);
        return vm->makePointer(renderer);
    }

    Value native_SDL_DestroyRenderer(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_DestroyRenderer expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_DestroyRenderer expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        SDL_DestroyRenderer(renderer);
        return vm->makeNil();
    }

    Value native_SDL_SetRenderDrawColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("SDL_SetRenderDrawColor expects 5 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_SetRenderDrawColor expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        Uint8 r = args[1].asNumber();
        Uint8 g = args[2].asNumber();
        Uint8 b = args[3].asNumber();
        Uint8 a = args[4].asNumber();

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        return vm->makeNil();
    }

    Value native_SDL_RenderClear(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_RenderClear expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderClear expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        SDL_RenderClear(renderer);
        return vm->makeNil();
    }

    Value native_SDL_RenderPresent(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_RenderPresent expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderPresent expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        SDL_RenderPresent(renderer);
        return vm->makeNil();
    }

    Value native_SDL_RenderDrawPoint(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("SDL_RenderDrawPoint expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderDrawPoint expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        int x = args[1].asNumber();
        int y = args[2].asNumber();

        SDL_RenderDrawPoint(renderer, x, y);
        return vm->makeNil();
    }

    Value native_SDL_RenderDrawLine(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("SDL_RenderDrawLine expects 5 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderDrawLine expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        int x1 = args[1].asNumber();
        int y1 = args[2].asNumber();
        int x2 = args[3].asNumber();
        int y2 = args[4].asNumber();

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        return vm->makeNil();
    }

    Value native_SDL_RenderDrawRect(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("SDL_RenderDrawRect expects 5 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderDrawRect expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        SDL_Rect rect;
        rect.x = args[1].asNumber();
        rect.y = args[2].asNumber();
        rect.w = args[3].asNumber();
        rect.h = args[4].asNumber();

        SDL_RenderDrawRect(renderer, &rect);
        return vm->makeNil();
    }

    Value native_SDL_RenderFillRect(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("SDL_RenderFillRect expects 5 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderFillRect expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        SDL_Rect rect;
        rect.x = args[1].asNumber();
        rect.y = args[2].asNumber();
        rect.w = args[3].asNumber();
        rect.h = args[4].asNumber();

        SDL_RenderFillRect(renderer, &rect);
        return vm->makeNil();
    }

    Value native_SDL_SetRenderDrawBlendMode(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SDL_SetRenderDrawBlendMode expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_SetRenderDrawBlendMode expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        SDL_BlendMode blendMode = (SDL_BlendMode)args[1].asNumber();

        SDL_SetRenderDrawBlendMode(renderer, blendMode);
        return vm->makeNil();
    }

    Value native_SDL_RenderSetScale(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("SDL_RenderSetScale expects 3 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderSetScale expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        float scaleX = args[1].asNumber();
        float scaleY = args[2].asNumber();

        SDL_RenderSetScale(renderer, scaleX, scaleY);
        return vm->makeNil();
    }

    Value native_SDL_RenderGetScale(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_RenderGetScale expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_RenderGetScale expects renderer pointer");
            return vm->makeNil();
        }

        SDL_Renderer *renderer = (SDL_Renderer *)args[0].asPointer();
        float scaleX, scaleY;
        SDL_RenderGetScale(renderer, &scaleX, &scaleY);

        Value result = vm->makeMap();
        MapInstance *map = result.asMap();
        map->table.set(vm->makeString("x").asString(), vm->makeDouble(scaleX));
        map->table.set(vm->makeString("y").asString(), vm->makeDouble(scaleY));
        return result;
    }
    void register_renderer(ModuleBuilder &module)
    {
        // =============================================================
        //  RENDERER FLAGS
        // =============================================================
        module.addInt("SDL_RENDERER_SOFTWARE", SDL_RENDERER_SOFTWARE)
            .addInt("SDL_RENDERER_ACCELERATED", SDL_RENDERER_ACCELERATED)
            .addInt("SDL_RENDERER_PRESENTVSYNC", SDL_RENDERER_PRESENTVSYNC)
            .addInt("SDL_RENDERER_TARGETTEXTURE", SDL_RENDERER_TARGETTEXTURE)

            // =============================================================
            //  BLEND MODES
            // =============================================================
            .addInt("SDL_BLENDMODE_NONE", SDL_BLENDMODE_NONE)
            .addInt("SDL_BLENDMODE_BLEND", SDL_BLENDMODE_BLEND)
            .addInt("SDL_BLENDMODE_ADD", SDL_BLENDMODE_ADD)
            .addInt("SDL_BLENDMODE_MOD", SDL_BLENDMODE_MOD)

            // =============================================================
            //  FUNCTIONS
            // =============================================================
            .addFunction("SDL_CreateRenderer", native_SDL_CreateRenderer, 3)
            .addFunction("SDL_DestroyRenderer", native_SDL_DestroyRenderer, 1)
            .addFunction("SDL_SetRenderDrawColor", native_SDL_SetRenderDrawColor, 5)
            .addFunction("SDL_RenderClear", native_SDL_RenderClear, 1)
            .addFunction("SDL_RenderPresent", native_SDL_RenderPresent, 1)
            .addFunction("SDL_RenderDrawPoint", native_SDL_RenderDrawPoint, 3)
            .addFunction("SDL_RenderDrawLine", native_SDL_RenderDrawLine, 5)
            .addFunction("SDL_RenderDrawRect", native_SDL_RenderDrawRect, 5)
            .addFunction("SDL_RenderFillRect", native_SDL_RenderFillRect, 5)
            .addFunction("SDL_SetRenderDrawBlendMode", native_SDL_SetRenderDrawBlendMode, 2)
            .addFunction("SDL_RenderSetScale", native_SDL_RenderSetScale, 3)
            .addFunction("SDL_RenderGetScale", native_SDL_RenderGetScale, 1);
    }
}