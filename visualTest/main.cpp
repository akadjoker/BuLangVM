// ============================================
// BuLang Visual Process Stress Test
// Embeds BuLang VM + Raylib to test processes
// in a real game loop with spawning/killing.
// ============================================

#include "interpreter.hpp"
#include "version.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>

#include "raylib.h"

// ── File loader (same as main bulang) ───────────────────────

static std::string readFileStr(const char *path)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

static std::string loaderBuffer;
static const char *fileLoader(const char *filename, size_t *outSize, void *userdata)
{
    if (!filename || !outSize) return nullptr;
    *outSize = 0;
    loaderBuffer = readFileStr(filename);
    if (loaderBuffer.empty()) return nullptr;
    *outSize = loaderBuffer.size();
    return loaderBuffer.c_str();
}

// ── Color native struct (POD: r, g, b, a as bytes) ─────────

struct BuColor { uint8_t r, g, b, a; };

static void color_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
{
    BuColor *c = static_cast<BuColor *>(buffer);
    c->r = (argc > 0) ? (uint8_t)args[0].asInt() : 255;
    c->g = (argc > 1) ? (uint8_t)args[1].asInt() : 255;
    c->b = (argc > 2) ? (uint8_t)args[2].asInt() : 255;
    c->a = (argc > 3) ? (uint8_t)args[3].asInt() : 255;
}

static Color buColorToRaylib(const BuColor *c)
{
    return (Color){c->r, c->g, c->b, c->a};
}

// ── Native functions exposed to BuLang ──────────────────────

// draw_circle_color(x, y, radius, color_struct)
static int native_draw_circle_color(Interpreter *vm, int argc, Value *args)
{
    if (argc < 4) { vm->runtimeError("draw_circle_color(x,y,radius,color)"); return 0; }
    float x = (float)args[0].asNumber();
    float y = (float)args[1].asNumber();
    float r = (float)args[2].asNumber();
    NativeStructInstance *ns = args[3].asNativeStructInstance();
    if (!ns || !ns->data) { vm->runtimeError("draw_circle_color: invalid Color (freed?)"); return 0; }
    BuColor *c = static_cast<BuColor *>(ns->data);
    DrawCircle((int)x, (int)y, r, buColorToRaylib(c));
    return 0;
}

// draw_rect_color(x, y, w, h, color_struct)
static int native_draw_rect_color(Interpreter *vm, int argc, Value *args)
{
    if (argc < 5) { vm->runtimeError("draw_rect_color(x,y,w,h,color)"); return 0; }
    float x = (float)args[0].asNumber();
    float y = (float)args[1].asNumber();
    float w = (float)args[2].asNumber();
    float h = (float)args[3].asNumber();
    NativeStructInstance *ns = args[4].asNativeStructInstance();
    if (!ns || !ns->data) { vm->runtimeError("draw_rect_color: invalid Color (freed?)"); return 0; }
    BuColor *c = static_cast<BuColor *>(ns->data);
    DrawRectangle((int)x, (int)y, (int)w, (int)h, buColorToRaylib(c));
    return 0;
}

// draw_circle(x, y, radius, r, g, b)
static int native_draw_circle(Interpreter *vm, int argc, Value *args)
{
    if (argc < 6) { vm->runtimeError("draw_circle(x,y,radius,r,g,b)"); return 0; }
    float x  = (float)args[0].asNumber();
    float y  = (float)args[1].asNumber();
    float r  = (float)args[2].asNumber();
    int cr   = args[3].asInt();
    int cg   = args[4].asInt();
    int cb   = args[5].asInt();
    DrawCircle((int)x, (int)y, r, (Color){(uint8_t)cr, (uint8_t)cg, (uint8_t)cb, 255});
    return 0;
}

// draw_rect(x, y, w, h, r, g, b)
static int native_draw_rect(Interpreter *vm, int argc, Value *args)
{
    if (argc < 7) { vm->runtimeError("draw_rect(x,y,w,h,r,g,b)"); return 0; }
    float x = (float)args[0].asNumber();
    float y = (float)args[1].asNumber();
    float w = (float)args[2].asNumber();
    float h = (float)args[3].asNumber();
    int cr  = args[4].asInt();
    int cg  = args[5].asInt();
    int cb  = args[6].asInt();
    DrawRectangle((int)x, (int)y, (int)w, (int)h, (Color){(uint8_t)cr, (uint8_t)cg, (uint8_t)cb, 255});
    return 0;
}

// draw_text(text, x, y, size, r, g, b)
static int native_draw_text(Interpreter *vm, int argc, Value *args)
{
    if (argc < 7) { vm->runtimeError("draw_text(text,x,y,size,r,g,b)"); return 0; }
    const char *text = args[0].asString()->chars();
    int x    = args[1].asInt();
    int y    = args[2].asInt();
    int size = args[3].asInt();
    int cr   = args[4].asInt();
    int cg   = args[5].asInt();
    int cb   = args[6].asInt();
    DrawText(text, x, y, size, (Color){(uint8_t)cr, (uint8_t)cg, (uint8_t)cb, 255});
    return 0;
}

// screen_width() -> int
static int native_screen_width(Interpreter *vm, int argc, Value *args)
{
    vm->pushInt(GetScreenWidth());
    return 1;
}

// screen_height() -> int
static int native_screen_height(Interpreter *vm, int argc, Value *args)
{
    vm->pushInt(GetScreenHeight());
    return 1;
}

// random_int(min, max) -> int
static int native_random_int(Interpreter *vm, int argc, Value *args)
{
    if (argc < 2) { vm->runtimeError("random_int(min,max)"); return 0; }
    int lo = args[0].asInt();
    int hi = args[1].asInt();
    vm->pushInt(GetRandomValue(lo, hi));
    return 1;
}

// get_fps() -> int
static int native_get_fps(Interpreter *vm, int argc, Value *args)
{
    vm->pushInt(GetFPS());
    return 1;
}

// is_key_pressed(key) -> bool  (key = ascii code)
static int native_is_key_pressed(Interpreter *vm, int argc, Value *args)
{
    if (argc < 1) { vm->runtimeError("is_key_pressed(key)"); return 0; }
    int key = args[0].asInt();
    vm->pushInt(IsKeyPressed(key) ? 1 : 0);
    return 1;
}

// is_key_down(key) -> bool
static int native_is_key_down(Interpreter *vm, int argc, Value *args)
{
    if (argc < 1) { vm->runtimeError("is_key_down(key)"); return 0; }
    int key = args[0].asInt();
    vm->pushInt(IsKeyDown(key) ? 1 : 0);
    return 1;
}

// ── Hooks: render processes using their privates ────────────

static void onRender(Interpreter *vm, Process *proc)
{
    float x = (float)proc->privates[(int)PrivateIndex::X].asNumber();
    float y = (float)proc->privates[(int)PrivateIndex::Y].asNumber();
    int size = (int)proc->privates[(int)PrivateIndex::SIZE].asNumber();
    int graph = (int)proc->privates[(int)PrivateIndex::GRAPH].asNumber();
    int r = (int)proc->privates[(int)PrivateIndex::iRED].asNumber();
    int g = (int)proc->privates[(int)PrivateIndex::iGREEN].asNumber();
    int b = (int)proc->privates[(int)PrivateIndex::iBLUE].asNumber();
    int a = (int)proc->privates[(int)PrivateIndex::iALPHA].asNumber();

    float radius = size * 0.15f;
    if (radius < 2) radius = 2;

    Color color = {(uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a};

    // graph: 0=circle, 1=rect, default=circle
    if (graph == 1)
    {
        DrawRectangle((int)(x - radius), (int)(y - radius),
                      (int)(radius * 2), (int)(radius * 2), color);
    }
    else
    {
        DrawCircleV({x, y}, radius, color);
    }
}

// ── Main ────────────────────────────────────────────────────

int main(int argc, char *argv[])
{
    const char *scriptFile = "visualTest/test_processes.bu";
    const char *outputBytecode = nullptr;
    
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc)
            outputBytecode = argv[++i];
        else
            scriptFile = argv[i];
    }

    std::string source = readFileStr(scriptFile);
    if (source.empty())
    {
        fprintf(stderr, "Error: cannot open '%s'\n", scriptFile);
        return 1;
    }

    // Init Raylib
    const int W = 800, H = 600;
    InitWindow(W, H, "BuLang Visual Process Test");
    SetTargetFPS(60);

    // Init VM
    Interpreter vm;
    vm.registerAll();
    vm.setFileLoader(fileLoader, nullptr);

    // Register Color native struct
    NativeStructDef *colorDef = vm.registerNativeStruct(
        "Color", sizeof(BuColor), color_ctor, nullptr, false);
    vm.addStructField(colorDef, "r", offsetof(BuColor, r), FieldType::BYTE);
    vm.addStructField(colorDef, "g", offsetof(BuColor, g), FieldType::BYTE);
    vm.addStructField(colorDef, "b", offsetof(BuColor, b), FieldType::BYTE);
    vm.addStructField(colorDef, "a", offsetof(BuColor, a), FieldType::BYTE);

    // Register raylib bindings
    vm.registerNative("draw_circle",       native_draw_circle,       6);
    vm.registerNative("draw_rect",         native_draw_rect,         7);
    vm.registerNative("draw_text",         native_draw_text,         7);
    vm.registerNative("draw_circle_color", native_draw_circle_color, 4);
    vm.registerNative("draw_rect_color",   native_draw_rect_color,   5);
    vm.registerNative("screen_width",   native_screen_width,   0);
    vm.registerNative("screen_height",  native_screen_height,  0);
    vm.registerNative("random_int",     native_random_int,     2);
    vm.registerNative("get_fps",        native_get_fps,        0);
    vm.registerNative("is_key_pressed", native_is_key_pressed, 1);
    vm.registerNative("is_key_down",    native_is_key_down,    1);

    // Set hooks
    VMHooks hooks = {};
    hooks.onRender = onRender;
    vm.setHooks(hooks);

    // Compile-only mode: export bytecode and exit (no window needed)
    if (outputBytecode)
    {
        // Compile only (don't execute)
        if (!vm.run(source.c_str(), false))
        {
            fprintf(stderr, "Error: compilation failed\n");
            CloseWindow();
            return 1;
        }
        if (vm.saveBytecode(outputBytecode))
        {
            printf("Compiled to: %s\n", outputBytecode);
            CloseWindow();
            return 0;
        }
        else
        {
            fprintf(stderr, "Error: failed to save bytecode to '%s'\n", outputBytecode);
            CloseWindow();
            return 1;
        }
    }

    // Detect .buc (bytecode) vs .bu (source)
    bool ok = false;
    std::string path(scriptFile);
    bool isBytecode = (path.size() > 4 && path.substr(path.size() - 4) == ".buc");

    if (isBytecode)
    {
        // loadBytecode already bootstraps: spawns main process and runs init code
        ok = vm.loadBytecode(scriptFile);
    }
    else
    {
        ok = vm.run(source.c_str(), false);
    }

    if (!ok)
    {
        fprintf(stderr, "Error: script %s failed\n", isBytecode ? "load" : "compilation/init");
        CloseWindow();
        return 1;
    }

    // Game loop
    bool renderEnabled = true;
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Toggle render with R key
        if (IsKeyPressed(KEY_R)) renderEnabled = !renderEnabled;

        // Step all BuLang processes
        vm.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        // Render all alive processes via hook
        if (renderEnabled)
            vm.render();

        // HUD
        char hud[128];
        snprintf(hud, sizeof(hud), "FPS: %d | Processes: %u | Render: %s",
                 GetFPS(), vm.getTotalAliveProcesses(),
                 renderEnabled ? "ON" : "OFF");
        DrawText(hud, 10, 10, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
