#include "bindings.hpp"
#include "imgui.h"
// #include "imgui_impl_opengl3.h"

// // Backend headers - included but initialized dynamically
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_sdl2.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif
#include <string>

namespace ImGuiBindings
{
    // Track which backend is active
    // static enum { BACKEND_NONE, BACKEND_GLFW, BACKEND_SDL } activeBackend = BACKEND_NONE;
    // static void* activeWindow = nullptr;

    // // =============================================================
    // // INITIALIZATION (Backend-specific via dlsym)
    // // =============================================================

    // Value native_imguiInitGLFW(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 1 || !args[0].isPointer())
    //     {
    //         Warning("imguiInitGLFW expects window pointer");
    //         return vm->makeBool(false);
    //     }

    //     #ifndef _WIN32
    //     // Check if GLFW is available
    //     void* glfwSym = dlsym(RTLD_DEFAULT, "glfwGetWindowUserPointer");
    //     if (!glfwSym)
    //     {
    //         Warning("ImGui: GLFW not found (load glfw plugin first)");
    //         return vm->makeBool(false);
    //     }
    //     #endif

    //     void* window = args[0].asPointer();
    //     bool installCallbacks = argc > 1 ? args[1].asBool() : true;

    //     // Create ImGui context
    //     IMGUI_CHECKVERSION();
    //     ImGui::CreateContext();

    //     ImGuiIO& io = ImGui::GetIO();
    //     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //     // Setup style
    //     ImGui::StyleColorsDark();

    //     // Initialize GLFW backend
    //     ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)window, installCallbacks);
    //     ImGui_ImplOpenGL3_Init("#version 330");

    //     activeBackend = BACKEND_GLFW;
    //     activeWindow = window;

    //     Info("ImGui: Initialized with GLFW backend");
    //     return vm->makeBool(true);
    // }

    // Value native_imguiInitSDL(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 2 || !args[0].isPointer() || !args[1].isPointer())
    //     {
    //         Warning("imguiInitSDL expects (window, gl_context)");
    //         return vm->makeBool(false);
    //     }

    //     #ifndef _WIN32
    //     // Check if SDL is available
    //     void* sdlSym = dlsym(RTLD_DEFAULT, "SDL_GetWindowID");
    //     if (!sdlSym)
    //     {
    //         Warning("ImGui: SDL not found (load sdl plugin first)");
    //         return vm->makeBool(false);
    //     }
    //     #endif

    //     void* window = args[0].asPointer();
    //     void* gl_context = args[1].asPointer();

    //     // Create ImGui context
    //     IMGUI_CHECKVERSION();
    //     ImGui::CreateContext();

    //     ImGuiIO& io = ImGui::GetIO();
    //     io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //     // Setup style
    //     ImGui::StyleColorsDark();

    //     // Initialize SDL backend
    //     ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)window, gl_context);
    //     ImGui_ImplOpenGL3_Init("#version 330");

    //     activeBackend = BACKEND_SDL;
    //     activeWindow = window;

    //     Info("ImGui: Initialized with SDL backend");
    //     return vm->makeBool(true);
    // }

    // Value native_imguiShutdown(Interpreter *vm, int argc, Value *args)
    // {
    //     ImGui_ImplOpenGL3_Shutdown();

    //     if (activeBackend == BACKEND_GLFW)
    //         ImGui_ImplGlfw_Shutdown();
    //     else if (activeBackend == BACKEND_SDL)
    //         ImGui_ImplSDL2_Shutdown();

    //     ImGui::DestroyContext();
    //     activeBackend = BACKEND_NONE;
    //     activeWindow = nullptr;

    //     Info("ImGui: Shutdown complete");
    //     return vm->makeNil();
    // }

    // // =============================================================
    // // FRAME
    // // =============================================================

    // Value native_imguiNewFrame(Interpreter *vm, int argc, Value *args)
    // {
    //     ImGui_ImplOpenGL3_NewFrame();

    //     if (activeBackend == BACKEND_GLFW)
    //         ImGui_ImplGlfw_NewFrame();
    //     else if (activeBackend == BACKEND_SDL)
    //         ImGui_ImplSDL2_NewFrame();

    //     ImGui::NewFrame();
    //     return vm->makeNil();
    // }

    // Value native_imguiRender(Interpreter *vm, int argc, Value *args)
    // {
    //     ImGui::Render();
    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //     return vm->makeNil();
    // }

    // Value native_imguiEndFrame(Interpreter *vm, int argc, Value *args)
    // {
    //     ImGui::EndFrame();
    //     return vm->makeNil();
    // }

    // // =============================================================
    // // SDL EVENT PROCESSING
    // // =============================================================

    // Value native_imguiProcessEventSDL(Interpreter *vm, int argc, Value *args)
    // {
    //     if (argc < 1 || !args[0].isPointer())
    //         return vm->makeBool(false);

    //     if (activeBackend != BACKEND_SDL)
    //     {
    //         Warning("imguiProcessEventSDL: SDL backend not active");
    //         return vm->makeBool(false);
    //     }

    //     SDL_Event* event = (SDL_Event*)args[0].asPointer();
    //     bool handled = ImGui_ImplSDL2_ProcessEvent(event);
    //     return vm->makeBool(handled);
    // }

    // =============================================================
    // WINDOWS
    // =============================================================

    Value native_imguiBegin(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);

        const char* name = args[0].asStringChars();
        int flags = argc > 1 ? (int)args[1].asNumber() : 0;

        bool open = true;
        bool result = ImGui::Begin(name, &open, flags);

        // // Return array [visible, open]
        // ArrayInstance* arr = vm->makeArray();
        // arr->values.push(vm->makeBool(result));
        // arr->values.push(vm->makeBool(open));
        // return vm->makeArray(arr);

        return vm->makeBool(result);
    }

    Value native_imguiEnd(Interpreter *vm, int argc, Value *args)
    {
        ImGui::End();
        return vm->makeNil();
    }

    Value native_imguiBeginChild(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);

        const char* id = args[0].asStringChars();
        float w = argc > 1 ? args[1].asNumber() : 0;
        float h = argc > 2 ? args[2].asNumber() : 0;
        bool border = argc > 3 ? args[3].asBool() : false;

        return vm->makeBool(ImGui::BeginChild(id, ImVec2(w, h), border));
    }

    Value native_imguiEndChild(Interpreter *vm, int argc, Value *args)
    {
        ImGui::EndChild();
        return vm->makeNil();
    }

    // =============================================================
    // WIDGETS - TEXT
    // =============================================================

    Value native_imguiText(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeNil();
        ImGui::Text("%s", args[0].asStringChars());
        return vm->makeNil();
    }

    Value native_imguiTextColored(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 5) return vm->makeNil();
        float r = args[0].asNumber();
        float g = args[1].asNumber();
        float b = args[2].asNumber();
        float a = args[3].asNumber();
        ImGui::TextColored(ImVec4(r, g, b, a), "%s", args[4].asStringChars());
        return vm->makeNil();
    }

    Value native_imguiTextWrapped(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeNil();
        ImGui::TextWrapped("%s", args[0].asStringChars());
        return vm->makeNil();
    }

    Value native_imguiLabelText(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeNil();
        ImGui::LabelText(args[0].asStringChars(), "%s", args[1].asStringChars());
        return vm->makeNil();
    }

    Value native_imguiBulletText(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeNil();
        ImGui::BulletText("%s", args[0].asStringChars());
        return vm->makeNil();
    }

    // =============================================================
    // WIDGETS - BUTTONS
    // =============================================================

    Value native_imguiButton(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);

        const char* label = args[0].asStringChars();
        float w = argc > 1 ? args[1].asNumber() : 0;
        float h = argc > 2 ? args[2].asNumber() : 0;

        return vm->makeBool(ImGui::Button(label, ImVec2(w, h)));
    }

    Value native_imguiSmallButton(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);
        return vm->makeBool(ImGui::SmallButton(args[0].asStringChars()));
    }

    Value native_imguiArrowButton(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeBool(false);
        return vm->makeBool(ImGui::ArrowButton(args[0].asStringChars(), (ImGuiDir)((int)args[1].asNumber())));
    }

    Value native_imguiCheckbox(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeBool(false);

        const char* label = args[0].asStringChars();
        bool v = args[1].asBool();
        bool changed = ImGui::Checkbox(label, &v);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeBool(v));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    Value native_imguiRadioButton(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeBool(false);
        return vm->makeBool(ImGui::RadioButton(args[0].asStringChars(), args[1].asBool()));
    }

    // =============================================================
    // WIDGETS - SLIDERS
    // =============================================================

    Value native_imguiSliderFloat(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 4) return vm->makeNil();

        const char* label = args[0].asStringChars();
        float v = args[1].asNumber();
        float min = args[2].asNumber();
        float max = args[3].asNumber();
        const char* format = argc > 4 ? args[4].asStringChars() : "%.3f";

        bool changed = ImGui::SliderFloat(label, &v, min, max, format);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeNumber(v));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    Value native_imguiSliderInt(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 4) return vm->makeNil();

        const char* label = args[0].asStringChars();
        int v = (int)args[1].asNumber();
        int min = (int)args[2].asNumber();
        int max = (int)args[3].asNumber();

        bool changed = ImGui::SliderInt(label, &v, min, max);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeInt(v));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    Value native_imguiSliderAngle(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeNil();

        const char* label = args[0].asStringChars();
        float v = args[1].asNumber();
        float min = argc > 2 ? args[2].asNumber() : -360.0f;
        float max = argc > 3 ? args[3].asNumber() : 360.0f;

        bool changed = ImGui::SliderAngle(label, &v, min, max);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeNumber(v));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    // =============================================================
    // WIDGETS - INPUT
    // =============================================================

    Value native_imguiInputText(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeNil();

        const char* label = args[0].asStringChars();
        const char* text = args[1].asStringChars();
        int flags = argc > 2 ? (int)args[2].asNumber() : 0;

        static char buf[256];
        strncpy(buf, text, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        bool changed = ImGui::InputText(label, buf, sizeof(buf), flags);

                // ArrayInstance* arr = vm->newArray();
                // arr->values.push_back(vm->makeBool(changed));
                // arr->values.push_back(vm->makeString(buf));
                // return vm->makeArray(arr);
                return vm->makeBool(changed);
    }

    Value native_imguiInputFloat(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeNil();

        const char* label = args[0].asStringChars();
        float v = args[1].asNumber();
        float step = argc > 2 ? args[2].asNumber() : 0.0f;
        float stepFast = argc > 3 ? args[3].asNumber() : 0.0f;

        bool changed = ImGui::InputFloat(label, &v, step, stepFast);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeNumber(v));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    Value native_imguiInputInt(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 2) return vm->makeNil();

        const char* label = args[0].asStringChars();
        int v = (int)args[1].asNumber();
        int step = argc > 2 ? (int)args[2].asNumber() : 1;
        int stepFast = argc > 3 ? (int)args[3].asNumber() : 100;

        bool changed = ImGui::InputInt(label, &v, step, stepFast);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeInt(v));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    // =============================================================
    // WIDGETS - COLOR
    // =============================================================

    Value native_imguiColorEdit3(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 4) return vm->makeNil();

        const char* label = args[0].asStringChars();
        float col[3] = {
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber()
        };

        bool changed = ImGui::ColorEdit3(label, col);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeNumber(col[0]));
        // arr->values.push_back(vm->makeNumber(col[1]));
        // arr->values.push_back(vm->makeNumber(col[2]));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    Value native_imguiColorEdit4(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 5) return vm->makeNil();

        const char* label = args[0].asStringChars();
        float col[4] = {
            (float)args[1].asNumber(),
            (float)args[2].asNumber(),
            (float)args[3].asNumber(),
            (float)args[4].asNumber()
        };

        bool changed = ImGui::ColorEdit4(label, col);

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeNumber(col[0]));
        // arr->values.push_back(vm->makeNumber(col[1]));
        // arr->values.push_back(vm->makeNumber(col[2]));
        // arr->values.push_back(vm->makeNumber(col[3]));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    // =============================================================
    // WIDGETS - COMBO/LIST
    // =============================================================

    Value native_imguiCombo(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 3 || !args[2].isArray()) return vm->makeNil();

        const char* label = args[0].asStringChars();
        int current = (int)args[1].asNumber();
        ArrayInstance* items = args[2].asArray();

        // Build items string
        std::string itemsStr;
        for (size_t i = 0; i < items->values.size(); i++)
        {
            if (items->values[i].isString())
                itemsStr += items->values[i].asStringChars();
            itemsStr += '\0';
        }
        itemsStr += '\0';

        bool changed = ImGui::Combo(label, &current, itemsStr.c_str());

        // ArrayInstance* arr = vm->newArray();
        // arr->values.push_back(vm->makeBool(changed));
        // arr->values.push_back(vm->makeInt(current));
        // return vm->makeArray(arr);
        return vm->makeBool(changed);
    }

    // =============================================================
    // LAYOUT
    // =============================================================

    Value native_imguiSeparator(Interpreter *vm, int argc, Value *args)
    {
        ImGui::Separator();
        return vm->makeNil();
    }

    Value native_imguiSameLine(Interpreter *vm, int argc, Value *args)
    {
        float offset = argc > 0 ? args[0].asNumber() : 0.0f;
        float spacing = argc > 1 ? args[1].asNumber() : -1.0f;
        ImGui::SameLine(offset, spacing);
        return vm->makeNil();
    }

    Value native_imguiNewLine(Interpreter *vm, int argc, Value *args)
    {
        ImGui::NewLine();
        return vm->makeNil();
    }

    Value native_imguiSpacing(Interpreter *vm, int argc, Value *args)
    {
        ImGui::Spacing();
        return vm->makeNil();
    }

    Value native_imguiDummy(Interpreter *vm, int argc, Value *args)
    {
        float w = argc > 0 ? args[0].asNumber() : 0;
        float h = argc > 1 ? args[1].asNumber() : 0;
        ImGui::Dummy(ImVec2(w, h));
        return vm->makeNil();
    }

    Value native_imguiIndent(Interpreter *vm, int argc, Value *args)
    {
        float w = argc > 0 ? args[0].asNumber() : 0;
        ImGui::Indent(w);
        return vm->makeNil();
    }

    Value native_imguiUnindent(Interpreter *vm, int argc, Value *args)
    {
        float w = argc > 0 ? args[0].asNumber() : 0;
        ImGui::Unindent(w);
        return vm->makeNil();
    }

    // =============================================================
    // TREES
    // =============================================================

    Value native_imguiTreeNode(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);
        return vm->makeBool(ImGui::TreeNode(args[0].asStringChars()));
    }

    Value native_imguiTreePop(Interpreter *vm, int argc, Value *args)
    {
        ImGui::TreePop();
        return vm->makeNil();
    }

    Value native_imguiCollapsingHeader(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);
        int flags = argc > 1 ? (int)args[1].asNumber() : 0;
        return vm->makeBool(ImGui::CollapsingHeader(args[0].asStringChars(), flags));
    }

    // =============================================================
    // POPUPS/MODALS
    // =============================================================

    Value native_imguiOpenPopup(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeNil();
        ImGui::OpenPopup(args[0].asStringChars());
        return vm->makeNil();
    }

    Value native_imguiBeginPopup(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);
        return vm->makeBool(ImGui::BeginPopup(args[0].asStringChars()));
    }

    Value native_imguiBeginPopupModal(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);
        return vm->makeBool(ImGui::BeginPopupModal(args[0].asStringChars()));
    }

    Value native_imguiEndPopup(Interpreter *vm, int argc, Value *args)
    {
        ImGui::EndPopup();
        return vm->makeNil();
    }

    Value native_imguiCloseCurrentPopup(Interpreter *vm, int argc, Value *args)
    {
        ImGui::CloseCurrentPopup();
        return vm->makeNil();
    }

    // =============================================================
    // MENUS
    // =============================================================

    Value native_imguiBeginMainMenuBar(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(ImGui::BeginMainMenuBar());
    }

    Value native_imguiEndMainMenuBar(Interpreter *vm, int argc, Value *args)
    {
        ImGui::EndMainMenuBar();
        return vm->makeNil();
    }

    Value native_imguiBeginMenu(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);
        bool enabled = argc > 1 ? args[1].asBool() : true;
        return vm->makeBool(ImGui::BeginMenu(args[0].asStringChars(), enabled));
    }

    Value native_imguiEndMenu(Interpreter *vm, int argc, Value *args)
    {
        ImGui::EndMenu();
        return vm->makeNil();
    }

    Value native_imguiMenuItem(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 1) return vm->makeBool(false);

        const char* label = args[0].asStringChars();
        const char* shortcut = argc > 1 && args[1].isString() ? args[1].asStringChars() : nullptr;
        bool selected = argc > 2 ? args[2].asBool() : false;
        bool enabled = argc > 3 ? args[3].asBool() : true;

        return vm->makeBool(ImGui::MenuItem(label, shortcut, selected, enabled));
    }

    // =============================================================
    // UTILITIES
    // =============================================================

    Value native_imguiIsItemHovered(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(ImGui::IsItemHovered());
    }

    Value native_imguiIsItemClicked(Interpreter *vm, int argc, Value *args)
    {
        int button = argc > 0 ? (int)args[0].asNumber() : 0;
        return vm->makeBool(ImGui::IsItemClicked(button));
    }

    Value native_imguiIsItemActive(Interpreter *vm, int argc, Value *args)
    {
        return vm->makeBool(ImGui::IsItemActive());
    }

    Value native_imguiSetItemDefaultFocus(Interpreter *vm, int argc, Value *args)
    {
        ImGui::SetItemDefaultFocus();
        return vm->makeNil();
    }

    Value native_imguiSetKeyboardFocusHere(Interpreter *vm, int argc, Value *args)
    {
        int offset = argc > 0 ? (int)args[0].asNumber() : 0;
        ImGui::SetKeyboardFocusHere(offset);
        return vm->makeNil();
    }

    Value native_imguiGetIO(Interpreter *vm, int argc, Value *args)
    {
        ImGuiIO& io = ImGui::GetIO();

        // MapInstance* map = vm->newMap();
        // map->set(vm->createString("deltaTime"), vm->makeNumber(io.DeltaTime));
        // map->set(vm->createString("framerate"), vm->makeNumber(io.Framerate));
        // map->set(vm->createString("wantCaptureMouse"), vm->makeBool(io.WantCaptureMouse));
        // map->set(vm->createString("wantCaptureKeyboard"), vm->makeBool(io.WantCaptureKeyboard));
        // map->set(vm->createString("wantTextInput"), vm->makeBool(io.WantTextInput));

        // return vm->makeMap(map);
        return vm->makeNil();
    }

    Value native_imguiShowDemoWindow(Interpreter *vm, int argc, Value *args)
    {
        ImGui::ShowDemoWindow();
        return vm->makeNil();
    }

    // =============================================================
    // STYLE
    // =============================================================

    Value native_imguiStyleColorsDark(Interpreter *vm, int argc, Value *args)
    {
        ImGui::StyleColorsDark();
        return vm->makeNil();
    }

    Value native_imguiStyleColorsLight(Interpreter *vm, int argc, Value *args)
    {
        ImGui::StyleColorsLight();
        return vm->makeNil();
    }

    Value native_imguiStyleColorsClassic(Interpreter *vm, int argc, Value *args)
    {
        ImGui::StyleColorsClassic();
        return vm->makeNil();
    }

    Value native_imguiPushStyleColor(Interpreter *vm, int argc, Value *args)
    {
        if (argc < 5) return vm->makeNil();
        int idx = (int)args[0].asNumber();
        ImGui::PushStyleColor(idx, ImVec4(args[1].asNumber(), args[2].asNumber(), args[3].asNumber(), args[4].asNumber()));
        return vm->makeNil();
    }

    Value native_imguiPopStyleColor(Interpreter *vm, int argc, Value *args)
    {
        int count = argc > 0 ? (int)args[0].asNumber() : 1;
        ImGui::PopStyleColor(count);
        return vm->makeNil();
    }

    // =============================================================
    // REGISTRATION
    // =============================================================

    void registerAll(Interpreter &vm)
    {
        ModuleBuilder module = vm.addModule("ImGui");

        module
            // Initialization
            // .addFunction("imguiInitGLFW", native_imguiInitGLFW, 2)
            // .addFunction("imguiInitSDL", native_imguiInitSDL, 2)
            // .addFunction("imguiShutdown", native_imguiShutdown, 0)

            // // Frame
            // .addFunction("imguiNewFrame", native_imguiNewFrame, 0)
            // .addFunction("imguiRender", native_imguiRender, 0)
            // .addFunction("imguiEndFrame", native_imguiEndFrame, 0)
            // .addFunction("imguiProcessEventSDL", native_imguiProcessEventSDL, 1)

            // Windows
            .addFunction("imguiBegin", native_imguiBegin, 2)
            .addFunction("imguiEnd", native_imguiEnd, 0)
            .addFunction("imguiBeginChild", native_imguiBeginChild, 4)
            .addFunction("imguiEndChild", native_imguiEndChild, 0)

            // Text
            .addFunction("imguiText", native_imguiText, 1)
            .addFunction("imguiTextColored", native_imguiTextColored, 5)
            .addFunction("imguiTextWrapped", native_imguiTextWrapped, 1)
            .addFunction("imguiLabelText", native_imguiLabelText, 2)
            .addFunction("imguiBulletText", native_imguiBulletText, 1)

            // Buttons
            .addFunction("imguiButton", native_imguiButton, 3)
            .addFunction("imguiSmallButton", native_imguiSmallButton, 1)
            .addFunction("imguiArrowButton", native_imguiArrowButton, 2)
            .addFunction("imguiCheckbox", native_imguiCheckbox, 2)
            .addFunction("imguiRadioButton", native_imguiRadioButton, 2)

            // Sliders
            .addFunction("imguiSliderFloat", native_imguiSliderFloat, 5)
            .addFunction("imguiSliderInt", native_imguiSliderInt, 4)
            .addFunction("imguiSliderAngle", native_imguiSliderAngle, 4)

            // Input
            .addFunction("imguiInputText", native_imguiInputText, 3)
            .addFunction("imguiInputFloat", native_imguiInputFloat, 4)
            .addFunction("imguiInputInt", native_imguiInputInt, 4)

            // Color
            .addFunction("imguiColorEdit3", native_imguiColorEdit3, 4)
            .addFunction("imguiColorEdit4", native_imguiColorEdit4, 5)

            // Combo
            .addFunction("imguiCombo", native_imguiCombo, 3)

            // Layout
            .addFunction("imguiSeparator", native_imguiSeparator, 0)
            .addFunction("imguiSameLine", native_imguiSameLine, 2)
            .addFunction("imguiNewLine", native_imguiNewLine, 0)
            .addFunction("imguiSpacing", native_imguiSpacing, 0)
            .addFunction("imguiDummy", native_imguiDummy, 2)
            .addFunction("imguiIndent", native_imguiIndent, 1)
            .addFunction("imguiUnindent", native_imguiUnindent, 1)

            // Trees
            .addFunction("imguiTreeNode", native_imguiTreeNode, 1)
            .addFunction("imguiTreePop", native_imguiTreePop, 0)
            .addFunction("imguiCollapsingHeader", native_imguiCollapsingHeader, 2)

            // Popups
            .addFunction("imguiOpenPopup", native_imguiOpenPopup, 1)
            .addFunction("imguiBeginPopup", native_imguiBeginPopup, 1)
            .addFunction("imguiBeginPopupModal", native_imguiBeginPopupModal, 1)
            .addFunction("imguiEndPopup", native_imguiEndPopup, 0)
            .addFunction("imguiCloseCurrentPopup", native_imguiCloseCurrentPopup, 0)

            // Menus
            .addFunction("imguiBeginMainMenuBar", native_imguiBeginMainMenuBar, 0)
            .addFunction("imguiEndMainMenuBar", native_imguiEndMainMenuBar, 0)
            .addFunction("imguiBeginMenu", native_imguiBeginMenu, 2)
            .addFunction("imguiEndMenu", native_imguiEndMenu, 0)
            .addFunction("imguiMenuItem", native_imguiMenuItem, 4)

            // Utilities
            .addFunction("imguiIsItemHovered", native_imguiIsItemHovered, 0)
            .addFunction("imguiIsItemClicked", native_imguiIsItemClicked, 1)
            .addFunction("imguiIsItemActive", native_imguiIsItemActive, 0)
            .addFunction("imguiSetItemDefaultFocus", native_imguiSetItemDefaultFocus, 0)
            .addFunction("imguiSetKeyboardFocusHere", native_imguiSetKeyboardFocusHere, 1)
            .addFunction("imguiGetIO", native_imguiGetIO, 0)
            .addFunction("imguiShowDemoWindow", native_imguiShowDemoWindow, 0)

            // Style
            .addFunction("imguiStyleColorsDark", native_imguiStyleColorsDark, 0)
            .addFunction("imguiStyleColorsLight", native_imguiStyleColorsLight, 0)
            .addFunction("imguiStyleColorsClassic", native_imguiStyleColorsClassic, 0)
            .addFunction("imguiPushStyleColor", native_imguiPushStyleColor, 5)
            .addFunction("imguiPopStyleColor", native_imguiPopStyleColor, 1)

            // Window Flags
            .addInt("ImGuiWindowFlags_None", ImGuiWindowFlags_None)
            .addInt("ImGuiWindowFlags_NoTitleBar", ImGuiWindowFlags_NoTitleBar)
            .addInt("ImGuiWindowFlags_NoResize", ImGuiWindowFlags_NoResize)
            .addInt("ImGuiWindowFlags_NoMove", ImGuiWindowFlags_NoMove)
            .addInt("ImGuiWindowFlags_NoScrollbar", ImGuiWindowFlags_NoScrollbar)
            .addInt("ImGuiWindowFlags_NoCollapse", ImGuiWindowFlags_NoCollapse)
            .addInt("ImGuiWindowFlags_AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize)
            .addInt("ImGuiWindowFlags_NoBackground", ImGuiWindowFlags_NoBackground)
            .addInt("ImGuiWindowFlags_MenuBar", ImGuiWindowFlags_MenuBar)

            // Direction
            .addInt("ImGuiDir_Left", ImGuiDir_Left)
            .addInt("ImGuiDir_Right", ImGuiDir_Right)
            .addInt("ImGuiDir_Up", ImGuiDir_Up)
            .addInt("ImGuiDir_Down", ImGuiDir_Down)

            // Input Text Flags
            .addInt("ImGuiInputTextFlags_None", ImGuiInputTextFlags_None)
            .addInt("ImGuiInputTextFlags_CharsDecimal", ImGuiInputTextFlags_CharsDecimal)
            .addInt("ImGuiInputTextFlags_CharsHexadecimal", ImGuiInputTextFlags_CharsHexadecimal)
            .addInt("ImGuiInputTextFlags_CharsUppercase", ImGuiInputTextFlags_CharsUppercase)
            .addInt("ImGuiInputTextFlags_CharsNoBlank", ImGuiInputTextFlags_CharsNoBlank)
            .addInt("ImGuiInputTextFlags_AutoSelectAll", ImGuiInputTextFlags_AutoSelectAll)
            .addInt("ImGuiInputTextFlags_EnterReturnsTrue", ImGuiInputTextFlags_EnterReturnsTrue)
            .addInt("ImGuiInputTextFlags_Password", ImGuiInputTextFlags_Password)
            .addInt("ImGuiInputTextFlags_ReadOnly", ImGuiInputTextFlags_ReadOnly);
    }
}
