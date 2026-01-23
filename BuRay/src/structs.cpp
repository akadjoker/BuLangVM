#include "bidings.hpp"

namespace RaylibBindings
{

    // ========================================
    // VECTOR2
    // ========================================

    void registerVector2(Interpreter &vm)
    {
        auto *vec2 = vm.registerNativeStruct(
            "Vector2",
            sizeof(Vector2),
            nullptr,
            nullptr);

        vm.addStructField(vec2, "x", offsetof(Vector2, x), FieldType::FLOAT);
        vm.addStructField(vec2, "y", offsetof(Vector2, y), FieldType::FLOAT);
    }

    // ========================================
    // VECTOR3
    // ========================================

    void registerVector3(Interpreter &vm)
    {
        auto *vec3 = vm.registerNativeStruct(
            "Vector3",
            sizeof(Vector3),
            nullptr,
            nullptr);

        vm.addStructField(vec3, "x", offsetof(Vector3, x), FieldType::FLOAT);
        vm.addStructField(vec3, "y", offsetof(Vector3, y), FieldType::FLOAT);
        vm.addStructField(vec3, "z", offsetof(Vector3, z), FieldType::FLOAT);
    }

    // ========================================
    // RECTANGLE
    // ========================================

    static void rectangle_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
    {
        Rectangle *v = (Rectangle *)buffer;
        v->x = 0;
        v->y = 0;
        v->width = 0;
        v->height = 0;
    }

    void registerRectangle(Interpreter &vm)
    {
        auto *rect = vm.registerNativeStruct(
            "Rectangle",
            sizeof(Rectangle),
            rectangle_ctor,
            nullptr);

        vm.addStructField(rect, "x", offsetof(Rectangle, x), FieldType::FLOAT);
        vm.addStructField(rect, "y", offsetof(Rectangle, y), FieldType::FLOAT);
        vm.addStructField(rect, "width", offsetof(Rectangle, width), FieldType::FLOAT);
        vm.addStructField(rect, "height", offsetof(Rectangle, height), FieldType::FLOAT);
    }

    // ========================================
    // COLOR
    // ========================================

    static void color_ctor(Interpreter *vm, void *buffer, int argc, Value *args)
    {
        Color *v = (Color *)buffer;
        v->r = (uint8)args[0].asNumber();
        v->g = (uint8)args[1].asNumber();
        v->b = (uint8)args[2].asNumber();
        v->a = (uint8)args[3].asNumber();
    }

    void registerColor(Interpreter &vm)
    {
        auto *color = vm.registerNativeStruct(
            "Color",
            sizeof(Color),
            color_ctor,
            nullptr);

        vm.addStructField(color, "r", offsetof(Color, r), FieldType::BYTE);
        vm.addStructField(color, "g", offsetof(Color, g), FieldType::BYTE);
        vm.addStructField(color, "b", offsetof(Color, b), FieldType::BYTE);
        vm.addStructField(color, "a", offsetof(Color, a), FieldType::BYTE);
    }

    // ========================================
    // CAMERA2D
    // ========================================

    void registerCamera2D(Interpreter &vm)
    {
        auto *camera = vm.registerNativeStruct(
            "Camera2D",
            sizeof(Camera2D),
            nullptr,
            nullptr);

        vm.addStructField(camera, "offset", offsetof(Camera2D, offset), FieldType::POINTER);
        vm.addStructField(camera, "target", offsetof(Camera2D, target), FieldType::POINTER);
        vm.addStructField(camera, "rotation", offsetof(Camera2D, rotation), FieldType::FLOAT);
        vm.addStructField(camera, "zoom", offsetof(Camera2D, zoom), FieldType::FLOAT);
    }

    // ========================================
    // REGISTER ALL STRUCTS
    // ========================================

    void register_structs(Interpreter &vm)
    {
        registerVector2(vm);
        registerVector3(vm);
        registerRectangle(vm);
        registerColor(vm);
        registerCamera2D(vm);
    }

}
