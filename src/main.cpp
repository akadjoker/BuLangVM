
#include "pch.h"
#include <pthread.h>

#include "Config.hpp"
#include "Utils.hpp"
#include "Vector.hpp"
#include "String.hpp"
#include "Raii.hpp"
#include "Map.hpp"
#include "Stack.hpp"
#include "Types.hpp"
#include "Vm.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "raylib.h"

bool Halt = false;
bool done = false;
    const int screenWidth = 800;
    const int screenHeight = 450;

void Native_TraceLog(int logLevel, const char *text, va_list args)
{
}

float memoryInMB(size_t bytes)
{
    return static_cast<float>(bytes) / (1024.0f * 1024.0f);
}

float memoryInKB(size_t bytes)
{
    return static_cast<float>(bytes) / 1024.0f;
}

const char *memoryIn(size_t bytes)
{
    if (bytes >= 1.0e6)
    {
        return TextFormat("%.2f MB", memoryInMB(bytes));
    }
    else if (bytes >= 1.0e3)
    {
        return TextFormat("%.2f KB", memoryInKB(bytes));
    }
    return TextFormat("%zu bytes", bytes);
}

inline void PrintValue(Value value)
{
    if (IS_STRING(value))
        printf("%s\n", AS_STRING(value)->string.c_str());
    else if (IS_NUMBER(value))
        printf("%g\n", AS_NUMBER(value));
    else if (IS_BOOLEAN(value))
        printf("%s\n", AS_BOOLEAN(value) ? "true" : "false");
    else if (IS_NONE(value))
        printf("none");
}

static int native_write(VirtualMachine *vm, int argc, Value *args)
{
    for (int i = 0; i < argc; i++)
    {
        Value value = args[i];
        if (IS_STRING(value))
            printf("%s", AS_STRING(value)->string.c_str());
        else if (IS_NUMBER(value))
            printf("%g", AS_NUMBER(value));
        else if (IS_BOOLEAN(value))
            printf("%s", AS_BOOLEAN(value) ? "true" : "false");
        else if (IS_NONE(value))
            printf("none");
    }
    return 0;
}
static int native_writeln(VirtualMachine *vm, int argc, Value *args)
{
    for (int i = 0; i < argc; i++)
    {
        Value value = args[i];
        if (IS_STRING(value))
            printf("%s", AS_STRING(value)->string.c_str());
        else if (IS_NUMBER(value))
            printf("%g", AS_NUMBER(value));
        else if (IS_BOOLEAN(value))
            printf("%s", AS_BOOLEAN(value) ? "true" : "false");
        else if (IS_NONE(value))
            printf("none");
    }
    printf("\n");
    return 0;
}

static int native_clock(VirtualMachine *vm, int argc, Value *args)
{
    Value v = NUMBER((double)clock() / CLOCKS_PER_SEC);

    vm->push(std::move(v));

    return 1;
}

static int native_to_int(VirtualMachine *vm, int argc, Value *args)
{

    if (IS_NUMBER(args[0]))
    {
        int value = (int)AS_NUMBER(args[0]);
        double number = static_cast<double>(value);
        vm->push(NUMBER(number));
    }
    else if (IS_BOOLEAN(args[0]))
    {
        double number = AS_BOOLEAN(args[0]) ? 1 : 0;
        vm->push(NUMBER(number));
    }
    else if (IS_STRING(args[0]))
    {
        double number = atof(AS_STRING(args[0])->string.c_str());
        vm->push(NUMBER(number));
    }
    return 1;
}

static int native_to_string(VirtualMachine *vm, int argc, Value *args)
{
    if (IS_NUMBER(args[0]))
    {
        String value = String(AS_NUMBER(args[0]));
        vm->push(STRING(std::move(value)));
    }
    else if (IS_BOOLEAN(args[0]))
    {
        String value = String(AS_BOOLEAN(args[0]) ? "true" : "false");
        vm->push(STRING(std::move(value)));
    }
    else if (IS_STRING(args[0]))
    {
        const char *value = AS_RAW_STRING(args[0]);
        vm->push(STRING(value));
    }
    return 1;
}

static int native_load_graph(VirtualMachine *vm, int argc, Value *args)
{
    if (IS_STRING(args[0]))
    {
        // const char* value = AS_RAW_STRING(args[0]);

        // AS_STRING

        // LoadTexture(value);
        printf("Load texture %s \n", AS_STRING(args[0])->string.c_str());

        vm->push_bool(true);

        // vm->push_bool(true);
    }

    return 1;
}

static int nave_key_down(VirtualMachine *vm, int argc, Value *args)
{

    vm->push(BOOLEAN(IsKeyDown(AS_INTEGER(args[0]))));

    return 1;
}

static int nave_key_press(VirtualMachine *vm, int argc, Value *args)
{
    vm->push_bool(IsKeyPressed(AS_INTEGER(args[0])));
    return 1;
}

static int native_mouse_down(VirtualMachine *vm, int argc, Value *args)
{
    vm->push(BOOLEAN(IsMouseButtonDown(AS_INTEGER(args[0]))));
    return 1;
}

static int native_mouse_press(VirtualMachine *vm, int argc, Value *args)
{
    vm->push_bool(IsMouseButtonPressed(AS_INTEGER(args[0])));
    return 1;
}

static int native_mouse_x(VirtualMachine *vm, int argc, Value *args)
{
    vm->push(NUMBER((double)GetMouseX()));
    return 1;
}

static int native_mouse_y(VirtualMachine *vm, int argc, Value *args)
{
    vm->push(NUMBER((double)GetMouseY()));
    return 1;
}

double rand_scale = 1.0 / (1 << 16) / (1 << 16);
double Random()
{
  unsigned int lo = rand() & 0xfff;
  unsigned int mid = rand() & 0xfff;
  unsigned int hi = rand() & 0xff;
  double result = (lo | (mid << 12) | (hi << 24)) * rand_scale;
  return result;
}



static int native_rand(VirtualMachine *vm, int argc, Value *args)
{
    vm->push(NUMBER(Random()));
    return 1;
}

bool RunMachine(const String &text)
{
    VirtualMachine vm;
    vm.registerFunction("write", native_write, -1);
    vm.registerFunction("writeln", native_writeln, -1);
    vm.registerFunction("clock", native_clock, 0);
    vm.registerFunction("toInt", native_to_int, 1);
    vm.registerFunction("toString", native_to_string, 1);

    if (!vm.Compile(std::move(text)))
        return false;

    vm.Run();

    return true;
}


Texture2D bunnyTex;





void instance_create(Instance *instance)
{
   
  //  INFO("Create instance: %s at %d %d %d", instance->name.c_str(), instance->locals[IX], instance->locals[IY], instance->locals[IGRAPH]);
}
void instance_destroy(Instance *instance)
{
   
   // INFO("Destroy instance: %s at %d %d %d", instance->name.c_str(), instance->locals[IX], instance->locals[IY], instance->locals[IGRAPH]);
}

void process_exec(Instance *instance)
{
   //DrawCircle(instance->locals[IX], instance->locals[IY], 10, RED);
    DrawTexture(bunnyTex, instance->locals[IX], instance->locals[IY],WHITE);
    //INFO("Process exec instance: %s at %d %d %d", instance->name.c_str(), instance->locals[IX], instance->locals[IY], instance->locals[IGRAPH]);
}

int main_game()
{
    char *text = LoadFileText("main.pc");
    if (!text)
    {
        ERROR("Failed to load main.pc");
        return 1;
    }
    String str(text);
    FreeTextFile(text);

    VirtualMachine vm;
    vm.registerFunction("write", native_write, -1);
    vm.registerFunction("writeln", native_writeln, -1);
    vm.registerFunction("clock", native_clock, 0);
    vm.registerFunction("toInt", native_to_int, 1);
    vm.registerFunction("toString", native_to_string, 1);
    vm.registerFunction("loadGraph", native_load_graph, 1);

    vm.registerFunction("key_down", nave_key_down, 1);
    vm.registerFunction("key_press", nave_key_press, 1);

    vm.registerFunction("mouse_down", native_mouse_down, 1);
    vm.registerFunction("mouse_press", native_mouse_press, 1);

    vm.registerFunction("mouse_x", native_mouse_x, 0);
    vm.registerFunction("mouse_y", native_mouse_y, 0);

    vm.registerFunction("rand", native_rand, 0);

    vm.registerInteger("screenWidth", screenWidth);
    vm.registerInteger("screenHeight", screenHeight);

    vm.hooks.instance_create_hook = instance_create;
    vm.hooks.instance_destroy_hook = instance_destroy;
    vm.hooks.process_exec_hook = process_exec;



    SetTraceLogLevel(LOG_NONE);
    //   SetTraceLogCallback(Native_TraceLog);

    InitWindow(screenWidth, screenHeight, "BuLang with Raylib");
    SetTargetFPS(1000000);

    bunnyTex = LoadTexture("wabbit_alpha.png");

    bool sucess = vm.Compile(std::move(str));
    INFO("Sucess: %s", sucess ? "true" : "false");

   
   

    while (!WindowShouldClose() && sucess)
    {
        // if (!sucess) break;
       // if(done )
        //    break;

        BeginDrawing();

        ClearBackground(BLACK);

         if (sucess)
        {
           // vm.Run();
            vm.Update();
        }
        
        


        DrawRectangle(0, 0, 380, 50, GREEN);
        DrawFPS(10, 10);
        if (done)
        {
            DrawText("All tasks done", 10, 60, 20, RED);
        }
        DrawText(TextFormat("Objects: %d mem: %s", (int)Arena::as().get_total_objects(), memoryIn(Arena::as().get_bytes_allocated())), 10, 30, 20, RED);
   

        EndDrawing();
    }
    


    UnloadTexture(bunnyTex);
    CloseWindow();
    return 0;
}

int main_machine()
{

    char *text = LoadFileText("main.pc");
    if (!text)
    {
        ERROR("Failed to load main.pc");
        return 1;
    }
    String str(text);
    FreeTextFile(text);

    VirtualMachine vm;
    vm.registerFunction("write", native_write, -1);
    vm.registerFunction("writeln", native_writeln, -1);
    vm.registerFunction("clock", native_clock, 0);
    vm.registerFunction("toInt", native_to_int, 1);
    vm.registerFunction("toString", native_to_string, 1);
    vm.registerFunction("key_down", nave_key_down, 1);
    vm.registerFunction("key_press", nave_key_press, 1);

    vm.registerFunction("mouse_down", native_mouse_down, 1);
    vm.registerFunction("mouse_press", native_mouse_press, 1);

    bool sucess = vm.Compile(std::move(str));
    INFO("Sucess: %s", sucess ? "true" : "false");
    if (sucess)
    {
        vm.Run();
    }
    return 0;
}

int main()
{

     //return main_machine();
    return main_game();

    return 0;
}