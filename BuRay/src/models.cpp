#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // BASIC 3D SHAPES
    // ========================================

    static int native_DrawCube(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawCube expects 7 arguments (x, y, z, width, height, length, color)");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawCube expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float width = args[3].asNumber();
        float height = args[4].asNumber();
        float length = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCube({x, y, z}, width, height, length, *color);
        return 0;
    }

    static int native_DrawCubeWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawCubeWires expects 7 arguments");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawCubeWires expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float width = args[3].asNumber();
        float height = args[4].asNumber();
        float length = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCubeWires({x, y, z}, width, height, length, *color);
        return 0;
    }

    static int native_DrawSphere(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawSphere expects 5 arguments (x, y, z, radius, color)");
            return 0;
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawSphere expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float radius = args[3].asNumber();

        auto *colorInst = args[4].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawSphere({x, y, z}, radius, *color);
        return 0;
    }

    static int native_DrawSphereWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawSphereWires expects 7 arguments (x, y, z, radius, rings, slices, color)");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawSphereWires expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float radius = args[3].asNumber();
        int rings = args[4].asNumber();
        int slices = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawSphereWires({x, y, z}, radius, rings, slices, *color);
        return 0;
    }

    static int native_DrawCylinder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 8)
        {
            Error("DrawCylinder expects 8 arguments (x, y, z, radiusTop, radiusBottom, height, slices, color)");
            return 0;
        }
        if (!args[7].isNativeStructInstance())
        {
            Error("DrawCylinder expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float radiusTop = args[3].asNumber();
        float radiusBottom = args[4].asNumber();
        float height = args[5].asNumber();
        int slices = args[6].asNumber();

        auto *colorInst = args[7].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCylinder({x, y, z}, radiusTop, radiusBottom, height, slices, *color);
        return 0;
    }

    static int native_DrawPlane(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawPlane expects 6 arguments (x, y, z, width, length, color)");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawPlane expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();
        float width = args[3].asNumber();
        float length = args[4].asNumber();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawPlane({x, y, z}, {width, length}, *color);
        return 0;
    }

    static int native_DrawGrid(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawGrid expects 2 arguments (slices, spacing)");
            return 0;
        }
        int slices = args[0].asNumber();
        float spacing = args[1].asNumber();
        DrawGrid(slices, spacing);
        return 0;
    }

    static int native_DrawLine3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawLine3D expects 7 arguments (x1, y1, z1, x2, y2, z2, color)");
            return 0;
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawLine3D expects Color");
            return 0;
        }
        float x1 = args[0].asNumber();
        float y1 = args[1].asNumber();
        float z1 = args[2].asNumber();
        float x2 = args[3].asNumber();
        float y2 = args[4].asNumber();
        float z2 = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawLine3D({x1, y1, z1}, {x2, y2, z2}, *color);
        return 0;
    }

    static int native_DrawPoint3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawPoint3D expects 4 arguments (x, y, z, color)");
            return 0;
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawPoint3D expects Color");
            return 0;
        }
        float x = args[0].asNumber();
        float y = args[1].asNumber();
        float z = args[2].asNumber();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawPoint3D({x, y, z}, *color);
        return 0;
    }

    // ========================================
    // MODEL LOADING
    // ========================================

    static int native_LoadModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
        {
            Error("LoadModel expects filename");
            return 0;
        }
        const char *filename = args[0].asStringChars();
        Model model = LoadModel(filename);
        Model *ptr = new Model(model);
        vm->pushPointer(ptr);
        return 1;
    }

    static int native_UnloadModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return 0;
        Model *model = (Model *)args[0].asPointer();
        UnloadModel(*model);
        delete model;
        return 0;
    }

    static int native_DrawModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawModel expects 6 arguments (model, x, y, z, scale, tint)");
            return 0;
        }
        if (!args[0].isPointer())
        {
            Error("DrawModel expects Model");
            return 0;
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawModel expects Color tint");
            return 0;
        }

        Model *model = (Model *)args[0].asPointer();
        float x = args[1].asNumber();
        float y = args[2].asNumber();
        float z = args[3].asNumber();
        float scale = args[4].asNumber();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawModel(*model, {x, y, z}, scale, *tint);
        return 0;
    }

    static int native_DrawModelWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawModelWires expects 6 arguments");
            return 0;
        }
        if (!args[0].isPointer() || !args[5].isNativeStructInstance())
        {
            Error("DrawModelWires expects Model and Color");
            return 0;
        }

        Model *model = (Model *)args[0].asPointer();
        float x = args[1].asNumber();
        float y = args[2].asNumber();
        float z = args[3].asNumber();
        float scale = args[4].asNumber();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawModelWires(*model, {x, y, z}, scale, *tint);
        return 0;
    }

    // ========================================
    // MESH GENERATION
    // ========================================

    static int native_GenMeshCube(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshCube expects 3 arguments (width, height, length)");
            return 0;
        }
        float width = args[0].asNumber();
        float height = args[1].asNumber();
        float length = args[2].asNumber();

        Mesh mesh = GenMeshCube(width, height, length);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenMeshSphere(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshSphere expects 3 arguments (radius, rings, slices)");
            return 0;
        }
        float radius = args[0].asNumber();
        int rings = args[1].asNumber();
        int slices = args[2].asNumber();

        Mesh mesh = GenMeshSphere(radius, rings, slices);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenMeshPlane(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("GenMeshPlane expects 4 arguments (width, length, resX, resZ)");
            return 0;
        }
        float width = args[0].asNumber();
        float length = args[1].asNumber();
        int resX = args[2].asNumber();
        int resZ = args[3].asNumber();

        Mesh mesh = GenMeshPlane(width, length, resX, resZ);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    static int native_GenMeshCylinder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshCylinder expects 3 arguments (radius, height, slices)");
            return 0;
        }
        float radius = args[0].asNumber();
        float height = args[1].asNumber();
        int slices = args[2].asNumber();

        Mesh mesh = GenMeshCylinder(radius, height, slices);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        vm->push(vm->makePointer(ptr));
        return 1;
    }

    // ========================================
    // REGISTER MODELS
    // ========================================

    void register_models(ModuleBuilder &mod)
    {
        // Basic 3D shapes
        mod.addFunction("DrawCube", native_DrawCube, 7)
           .addFunction("DrawCubeWires", native_DrawCubeWires, 7)
           .addFunction("DrawSphere", native_DrawSphere, 5)
           .addFunction("DrawSphereWires", native_DrawSphereWires, 7)
           .addFunction("DrawCylinder", native_DrawCylinder, 8)
           .addFunction("DrawPlane", native_DrawPlane, 6)
           .addFunction("DrawGrid", native_DrawGrid, 2)
           .addFunction("DrawLine3D", native_DrawLine3D, 7)
           .addFunction("DrawPoint3D", native_DrawPoint3D, 4)

        // Model loading
           .addFunction("LoadModel", native_LoadModel, 1)
           .addFunction("UnloadModel", native_UnloadModel, 1)
           .addFunction("DrawModel", native_DrawModel, 6)
           .addFunction("DrawModelWires", native_DrawModelWires, 6)

        // Mesh generation (returns Model)
           .addFunction("GenMeshCube", native_GenMeshCube, 3)
           .addFunction("GenMeshSphere", native_GenMeshSphere, 3)
           .addFunction("GenMeshPlane", native_GenMeshPlane, 4)
           .addFunction("GenMeshCylinder", native_GenMeshCylinder, 3);
    }

}
