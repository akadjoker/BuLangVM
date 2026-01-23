#include "bidings.hpp"

namespace RaylibBindings
{

    // ========================================
    // CAMERA 2D
    // ========================================

    static Value native_BeginMode2D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isNativeStructInstance())
        {
            Error("BeginMode2D expects Camera2D");
            return vm->makeNil();
        }
        auto *inst = args[0].asNativeStructInstance();
        Camera2D *camera = (Camera2D *)inst->data;
        BeginMode2D(*camera);
        return vm->makeNil();
    }

    static Value native_EndMode2D(Interpreter *vm, int argc, Value *args)
    {
        EndMode2D();
        return vm->makeNil();
    }

    static Value native_GetScreenToWorld2D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("GetScreenToWorld2D expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isNativeStructInstance() || !args[1].isNativeStructInstance())
        {
            Error("GetScreenToWorld2D expects Vector2 and Camera2D");
            return vm->makeNil();
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;
        auto *camInst = args[1].asNativeStructInstance();
        Camera2D *camera = (Camera2D *)camInst->data;

        Vector2 result = GetScreenToWorld2D(*pos, *camera);
        // TODO: Return Vector2 struct instance
        return vm->makeNil();
    }

    static Value native_GetWorldToScreen2D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("GetWorldToScreen2D expects 2 arguments");
            return vm->makeNil();
        }
        if (!args[0].isNativeStructInstance() || !args[1].isNativeStructInstance())
        {
            Error("GetWorldToScreen2D expects Vector2 and Camera2D");
            return vm->makeNil();
        }
        auto *posInst = args[0].asNativeStructInstance();
        Vector2 *pos = (Vector2 *)posInst->data;
        auto *camInst = args[1].asNativeStructInstance();
        Camera2D *camera = (Camera2D *)camInst->data;

        Vector2 result = GetWorldToScreen2D(*pos, *camera);
        // TODO: Return Vector2 struct instance
        return vm->makeNil();
    }

    // ========================================
    // CAMERA 3D
    // ========================================

    static Value native_BeginMode3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
        {
            Error("BeginMode3D expects Camera3D pointer");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        BeginMode3D(*camera);
        return vm->makeNil();
    }

    static Value native_EndMode3D(Interpreter *vm, int argc, Value *args)
    {
        EndMode3D();
        return vm->makeNil();
    }

    static Value native_CreateCamera3D(Interpreter *vm, int argc, Value *args)
    {
        Camera3D *camera = new Camera3D();
        camera->position = {0.0f, 10.0f, 10.0f};
        camera->target = {0.0f, 0.0f, 0.0f};
        camera->up = {0.0f, 1.0f, 0.0f};
        camera->fovy = 45.0f;
        camera->projection = CAMERA_PERSPECTIVE;
        return vm->makePointer(camera);
    }

    static Value native_DestroyCamera3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        delete camera;
        return vm->makeNil();
    }

    static Value native_SetCamera3DPosition(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer())
        {
            Error("SetCamera3DPosition expects Camera3D and x, y, z");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        camera->position.x = args[1].asDouble();
        camera->position.y = args[2].asDouble();
        camera->position.z = args[3].asDouble();
        return vm->makeNil();
    }

    static Value native_SetCamera3DTarget(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer())
        {
            Error("SetCamera3DTarget expects Camera3D and x, y, z");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        camera->target.x = args[1].asDouble();
        camera->target.y = args[2].asDouble();
        camera->target.z = args[3].asDouble();
        return vm->makeNil();
    }

    static Value native_SetCamera3DUp(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4 || !args[0].isPointer())
        {
            Error("SetCamera3DUp expects Camera3D and x, y, z");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        camera->up.x = args[1].asDouble();
        camera->up.y = args[2].asDouble();
        camera->up.z = args[3].asDouble();
        return vm->makeNil();
    }

    static Value native_SetCamera3DFovy(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
        {
            Error("SetCamera3DFovy expects Camera3D and fovy");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        camera->fovy = args[1].asDouble();
        return vm->makeNil();
    }

    static Value native_SetCamera3DProjection(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
        {
            Error("SetCamera3DProjection expects Camera3D and projection");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        camera->projection = (int)args[1].asNumber();
        return vm->makeNil();
    }

    static Value native_UpdateCamera(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2 || !args[0].isPointer())
        {
            Error("UpdateCamera expects Camera3D and mode");
            return vm->makeNil();
        }
        Camera3D *camera = (Camera3D *)args[0].asPointer();
        int mode = args[1].asNumber();
        UpdateCamera(camera, mode);
        return vm->makeNil();
    }

    // ========================================
    // REGISTER CAMERA
    // ========================================

    void register_camera(ModuleBuilder &mod)
    {
        // Camera 2D
        mod.addFunction("BeginMode2D", native_BeginMode2D, 1)
           .addFunction("EndMode2D", native_EndMode2D, 0)

        // Camera 3D
           .addFunction("BeginMode3D", native_BeginMode3D, 1)
           .addFunction("EndMode3D", native_EndMode3D, 0)
           .addFunction("CreateCamera3D", native_CreateCamera3D, 0)
           .addFunction("DestroyCamera3D", native_DestroyCamera3D, 1)
           .addFunction("SetCamera3DPosition", native_SetCamera3DPosition, 4)
           .addFunction("SetCamera3DTarget", native_SetCamera3DTarget, 4)
           .addFunction("SetCamera3DUp", native_SetCamera3DUp, 4)
           .addFunction("SetCamera3DFovy", native_SetCamera3DFovy, 2)
           .addFunction("SetCamera3DProjection", native_SetCamera3DProjection, 2)
           .addFunction("UpdateCamera", native_UpdateCamera, 2)

        // Camera mode constants
           .addInt("CAMERA_CUSTOM", CAMERA_CUSTOM)
           .addInt("CAMERA_FREE", CAMERA_FREE)
           .addInt("CAMERA_ORBITAL", CAMERA_ORBITAL)
           .addInt("CAMERA_FIRST_PERSON", CAMERA_FIRST_PERSON)
           .addInt("CAMERA_THIRD_PERSON", CAMERA_THIRD_PERSON)

        // Projection constants
           .addInt("CAMERA_PERSPECTIVE", CAMERA_PERSPECTIVE)
           .addInt("CAMERA_ORTHOGRAPHIC", CAMERA_ORTHOGRAPHIC);
    }

}
