#include "bindings.hpp"

namespace PointBindings
{

    void registerPoint(Interpreter &vm)
    {
        auto *point = vm.registerNativeStruct(
            "Point",
            sizeof(Point),
            point_ctor,
            nullptr);

        vm.addStructField(point, "x", offsetof(Point, x), FieldType::FLOAT);
        vm.addStructField(point, "y", offsetof(Point, y), FieldType::FLOAT);
    }

    void registerAll(Interpreter &vm)
    {
        registerPoint(vm);
    }

} // namespace PointBindings

namespace Vector2DBindings
{

    // 1. Define your native data structure
    struct Vector2D
    {
        float x;
        float y;
    };

    // 2. Implement the constructor
    //    Returns a pointer to your native data (stored as void*)
    void *Vector2D_constructor(Interpreter *vm, int argCount, Value *args)
    {
        Vector2D *vec = new Vector2D();
        if (argCount >= 2)
        {
            vec->x = (float)args[0].asNumber();
            vec->y = (float)args[1].asNumber();
        }
        else
        {
            vec->x = 0.0f;
            vec->y = 0.0f;
        }
        Info("Vector2D constructed: (%f, %f)", vec->x, vec->y);
        return vec;
    }

    // 3. Implement the destructor
    //    Called automatically when the instance is garbage collected
    void Vector2D_destructor(Interpreter *vm, void *data)
    {
        Info("Vector2D destructed");
        delete static_cast<Vector2D *>(data);
    }

    // 4. Define property getters
    Value Vector2D_getX(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Vector2D *>(data)->x);
    }

    Value Vector2D_getY(Interpreter *vm, void *data)
    {
        return vm->makeFloat(static_cast<Vector2D *>(data)->y);
    }

    // 5. Define property setters
    void Vector2D_setX(Interpreter *vm, void *data, Value value)
    {
        static_cast<Vector2D *>(data)->x = (float)value.asNumber();
    }

    void Vector2D_setY(Interpreter *vm, void *data, Value value)
    {
        static_cast<Vector2D *>(data)->y = (float)value.asNumber();
    }

    // 6. Define methods
    int Vector2D_length(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Vector2D *vec = static_cast<Vector2D *>(data);
        float len = std::sqrt(vec->x * vec->x + vec->y * vec->y);

        return 1;
    }

    int Vector2D_add(Interpreter *vm, void *data, int argCount, Value *args)
    {
        Vector2D *self = static_cast<Vector2D *>(data);

        if (argCount >= 2)
        {
            self->x += (float)args[0].asNumber();
            self->y += (float)args[1].asNumber();
        }

        return 0;
    }

    void registerVector2D(Interpreter &vm)
    {

        NativeClassDef *vec2 = vm.registerNativeClass("Vector2D",
                                                      Vector2D_constructor,
                                                      Vector2D_destructor,
                                                      2,  // number of properties
                                                      false  // persistent (false = GC coleta)
        );

        // Add properties (name, getter, setter)
        vm.addNativeProperty(vec2, "x", Vector2D_getX, Vector2D_setX);
        vm.addNativeProperty(vec2, "y", Vector2D_getY, Vector2D_setY);

        // Add methods
        vm.addNativeMethod(vec2, "length", Vector2D_length);
        vm.addNativeMethod(vec2, "add", Vector2D_add);
    }

    void registerAll(Interpreter &vm)
    {
        registerVector2D(vm);
    }

} // namespace Vector2DBindings
