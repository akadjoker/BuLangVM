#include "bidings.hpp"

namespace SDLBindings
{

    Value native_SDL_CreateWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("SDL_CreateWindow expects 6 arguments");
            return vm->makeNil();
        }
        if (!args[0].isString())
        {
            Error("SDL_CreateWindow expects string title");
            return vm->makeNil();
        }

        const char *title = args[0].asStringChars();
        int x = args[1].asNumber();
        int y = args[2].asNumber();
        int w = args[3].asNumber();
        int h = args[4].asNumber();
        Uint32 flags = args[5].asNumber();

        SDL_Window *window = SDL_CreateWindow(title, x, y, w, h, flags);
        return vm->makePointer(window);
    }

    Value native_SDL_DestroyWindow(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_DestroyWindow expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_DestroyWindow expects window pointer");
            return vm->makeNil();
        }

        SDL_Window *window = (SDL_Window *)args[0].asPointer();
        SDL_DestroyWindow(window);
        return vm->makeNil();
    }

    Value native_SDL_PollEvent(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1)
        {
            Error("SDL_PollEvent expects 1 argument");
            return vm->makeNil();
        }
        if (!args[0].isNativeStructInstance())
        {
            Error("SDL_PollEvent expects SDL_Event");
            return vm->makeNil();
        }

        auto *eventInst = args[0].asNativeStructInstance();
        SDL_Event *event = (SDL_Event *)eventInst->data;

        int result = SDL_PollEvent(event);
        return vm->makeInt(result);
    }

    Value native_SDL_SetWindowTitle(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("SDL_SetWindowTitle expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("SDL_SetWindowTitle expects window pointer");
            return vm->makeNil();
        }

        SDL_Window *window = (SDL_Window *)args[0].asPointer();
        const char *title = args[1].asStringChars();
        SDL_SetWindowTitle(window, title);
        return vm->makeNil();
    }

    void register_window(ModuleBuilder &mod)
    {

        // =============================================================
        //  WINDOW CREATION: Positioning
        // =============================================================
        mod.addInt("SDL_WINDOWPOS_CENTERED", SDL_WINDOWPOS_CENTERED)
            .addInt("SDL_WINDOWPOS_UNDEFINED", SDL_WINDOWPOS_UNDEFINED)

            // =============================================================
            //  WINDOW CREATION: Flags (Combináveis com + ou | no script)
            // =============================================================
            .addInt("SDL_WINDOW_RESIZABLE", SDL_WINDOW_RESIZABLE)
            .addInt("SDL_WINDOW_FULLSCREEN", SDL_WINDOW_FULLSCREEN)
            .addInt("SDL_WINDOW_FULLSCREEN_DESKTOP", SDL_WINDOW_FULLSCREEN_DESKTOP)
            .addInt("SDL_WINDOW_OPENGL", SDL_WINDOW_OPENGL)
            .addInt("SDL_WINDOW_VULKAN", SDL_WINDOW_VULKAN)
            .addInt("SDL_WINDOW_SHOWN", SDL_WINDOW_SHOWN)
            .addInt("SDL_WINDOW_HIDDEN", SDL_WINDOW_HIDDEN)
            .addInt("SDL_WINDOW_BORDERLESS", SDL_WINDOW_BORDERLESS)
            .addInt("SDL_WINDOW_ALLOW_HIGHDPI", SDL_WINDOW_ALLOW_HIGHDPI)
            // =============================================================
            // 5. EVENT TYPES (Para comparar com event.type)
            // =============================================================
            .addInt("SDL_QUIT", SDL_QUIT) // 256
            .addInt("SDL_KEYDOWN", SDL_KEYDOWN)  //0x300
            .addInt("SDL_KEYUP", SDL_KEYUP)      //0x301
            .addInt("SDL_MOUSEMOTION", SDL_MOUSEMOTION) //0x400
            .addInt("SDL_MOUSEBUTTONDOWN", SDL_MOUSEBUTTONDOWN) //0x402
            .addInt("SDL_MOUSEBUTTONUP", SDL_MOUSEBUTTONUP) //0x403
            .addInt("SDL_WINDOWEVENT", SDL_WINDOWEVENT)  //0x200
            .addInt("SDL_MOUSEWHEEL", SDL_MOUSEWHEEL)           // 1027

            // Mouse Buttons (Para comparar com event.button)
            .addByte("SDL_BUTTON_LEFT", SDL_BUTTON_LEFT)
            .addByte("SDL_BUTTON_MIDDLE", SDL_BUTTON_MIDDLE)
            .addByte("SDL_BUTTON_RIGHT", SDL_BUTTON_RIGHT)

        // =============================================================
        // 6. WINDOW EVENTS (Para comparar com event.winEvent)
        // =============================================================
        .addInt("SDL_WINDOWEVENT_SHOWN", SDL_WINDOWEVENT_SHOWN)
            .addInt("SDL_WINDOWEVENT_HIDDEN", SDL_WINDOWEVENT_HIDDEN)
            .addInt("SDL_WINDOWEVENT_EXPOSED", SDL_WINDOWEVENT_EXPOSED)
            .addInt("SDL_WINDOWEVENT_MOVED", SDL_WINDOWEVENT_MOVED)
            .addInt("SDL_WINDOWEVENT_RESIZED", SDL_WINDOWEVENT_RESIZED)
            .addInt("SDL_WINDOWEVENT_SIZE_CHANGED", SDL_WINDOWEVENT_SIZE_CHANGED)
            .addInt("SDL_WINDOWEVENT_MINIMIZED", SDL_WINDOWEVENT_MINIMIZED)
            .addInt("SDL_WINDOWEVENT_MAXIMIZED", SDL_WINDOWEVENT_MAXIMIZED)
            .addInt("SDL_WINDOWEVENT_RESTORED", SDL_WINDOWEVENT_RESTORED)
            .addInt("SDL_WINDOWEVENT_ENTER", SDL_WINDOWEVENT_ENTER)
            .addInt("SDL_WINDOWEVENT_LEAVE", SDL_WINDOWEVENT_LEAVE)
            .addInt("SDL_WINDOWEVENT_FOCUS_GAINED", SDL_WINDOWEVENT_FOCUS_GAINED)
            .addInt("SDL_WINDOWEVENT_FOCUS_LOST", SDL_WINDOWEVENT_FOCUS_LOST)
            .addInt("SDL_WINDOWEVENT_CLOSE", SDL_WINDOWEVENT_CLOSE)
            // Nota: Removi alguns menos usados como ICCPROF para poupar espaço,
            // mas podem ser adicionados futuramente

            .addFunction("SDL_PollEvent", native_SDL_PollEvent, 1)

            .addFunction("SDL_CreateWindow", native_SDL_CreateWindow, 6)
            .addFunction("SDL_DestroyWindow", native_SDL_DestroyWindow, 1)
            .addFunction("SDL_SetWindowTitle", native_SDL_SetWindowTitle, 2);
    }

}