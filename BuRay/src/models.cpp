#include "bindings.hpp"

namespace RaylibBindings
{

    // ========================================
    // BASIC 3D SHAPES
    // ========================================

    static Value native_DrawCube(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawCube expects 7 arguments (x, y, z, width, height, length, color)");
            return vm->makeNil();
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawCube expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();
        float width = args[3].asDouble();
        float height = args[4].asDouble();
        float length = args[5].asDouble();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCube({x, y, z}, width, height, length, *color);
        return vm->makeNil();
    }

    static Value native_DrawCubeWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawCubeWires expects 7 arguments");
            return vm->makeNil();
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawCubeWires expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();
        float width = args[3].asDouble();
        float height = args[4].asDouble();
        float length = args[5].asDouble();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCubeWires({x, y, z}, width, height, length, *color);
        return vm->makeNil();
    }

    static Value native_DrawSphere(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 5)
        {
            Error("DrawSphere expects 5 arguments (x, y, z, radius, color)");
            return vm->makeNil();
        }
        if (!args[4].isNativeStructInstance())
        {
            Error("DrawSphere expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();
        float radius = args[3].asDouble();

        auto *colorInst = args[4].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawSphere({x, y, z}, radius, *color);
        return vm->makeNil();
    }

    static Value native_DrawSphereWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawSphereWires expects 7 arguments (x, y, z, radius, rings, slices, color)");
            return vm->makeNil();
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawSphereWires expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();
        float radius = args[3].asDouble();
        int rings = args[4].asNumber();
        int slices = args[5].asNumber();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawSphereWires({x, y, z}, radius, rings, slices, *color);
        return vm->makeNil();
    }

    static Value native_DrawCylinder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 8)
        {
            Error("DrawCylinder expects 8 arguments (x, y, z, radiusTop, radiusBottom, height, slices, color)");
            return vm->makeNil();
        }
        if (!args[7].isNativeStructInstance())
        {
            Error("DrawCylinder expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();
        float radiusTop = args[3].asDouble();
        float radiusBottom = args[4].asDouble();
        float height = args[5].asDouble();
        int slices = args[6].asNumber();

        auto *colorInst = args[7].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawCylinder({x, y, z}, radiusTop, radiusBottom, height, slices, *color);
        return vm->makeNil();
    }

    static Value native_DrawPlane(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawPlane expects 6 arguments (x, y, z, width, length, color)");
            return vm->makeNil();
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawPlane expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();
        float width = args[3].asDouble();
        float length = args[4].asDouble();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawPlane({x, y, z}, {width, length}, *color);
        return vm->makeNil();
    }

    static Value native_DrawGrid(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 2)
        {
            Error("DrawGrid expects 2 arguments (slices, spacing)");
            return vm->makeNil();
        }
        int slices = args[0].asNumber();
        float spacing = args[1].asDouble();
        DrawGrid(slices, spacing);
        return vm->makeNil();
    }

    static Value native_DrawLine3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 7)
        {
            Error("DrawLine3D expects 7 arguments (x1, y1, z1, x2, y2, z2, color)");
            return vm->makeNil();
        }
        if (!args[6].isNativeStructInstance())
        {
            Error("DrawLine3D expects Color");
            return vm->makeNil();
        }
        float x1 = args[0].asDouble();
        float y1 = args[1].asDouble();
        float z1 = args[2].asDouble();
        float x2 = args[3].asDouble();
        float y2 = args[4].asDouble();
        float z2 = args[5].asDouble();

        auto *colorInst = args[6].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawLine3D({x1, y1, z1}, {x2, y2, z2}, *color);
        return vm->makeNil();
    }

    static Value native_DrawPoint3D(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("DrawPoint3D expects 4 arguments (x, y, z, color)");
            return vm->makeNil();
        }
        if (!args[3].isNativeStructInstance())
        {
            Error("DrawPoint3D expects Color");
            return vm->makeNil();
        }
        float x = args[0].asDouble();
        float y = args[1].asDouble();
        float z = args[2].asDouble();

        auto *colorInst = args[3].asNativeStructInstance();
        Color *color = (Color *)colorInst->data;

        DrawPoint3D({x, y, z}, *color);
        return vm->makeNil();
    }

    // ========================================
    // MODEL LOADING
    // ========================================

    static Value native_LoadModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isString())
        {
            Error("LoadModel expects filename");
            return vm->makeNil();
        }
        const char *filename = args[0].asStringChars();
        Model model = LoadModel(filename);
        Model *ptr = new Model(model);
        return vm->makePointer(ptr);
    }

    static Value native_UnloadModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 1 || !args[0].isPointer())
            return vm->makeNil();
        Model *model = (Model *)args[0].asPointer();
        UnloadModel(*model);
        delete model;
        return vm->makeNil();
    }

    static Value native_DrawModel(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawModel expects 6 arguments (model, x, y, z, scale, tint)");
            return vm->makeNil();
        }
        if (!args[0].isPointer())
        {
            Error("DrawModel expects Model");
            return vm->makeNil();
        }
        if (!args[5].isNativeStructInstance())
        {
            Error("DrawModel expects Color tint");
            return vm->makeNil();
        }

        Model *model = (Model *)args[0].asPointer();
        float x = args[1].asDouble();
        float y = args[2].asDouble();
        float z = args[3].asDouble();
        float scale = args[4].asDouble();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawModel(*model, {x, y, z}, scale, *tint);
        return vm->makeNil();
    }

    static Value native_DrawModelWires(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 6)
        {
            Error("DrawModelWires expects 6 arguments");
            return vm->makeNil();
        }
        if (!args[0].isPointer() || !args[5].isNativeStructInstance())
        {
            Error("DrawModelWires expects Model and Color");
            return vm->makeNil();
        }

        Model *model = (Model *)args[0].asPointer();
        float x = args[1].asDouble();
        float y = args[2].asDouble();
        float z = args[3].asDouble();
        float scale = args[4].asDouble();

        auto *colorInst = args[5].asNativeStructInstance();
        Color *tint = (Color *)colorInst->data;

        DrawModelWires(*model, {x, y, z}, scale, *tint);
        return vm->makeNil();
    }

    // ========================================
    // MESH GENERATION
    // ========================================

    static Value native_GenMeshCube(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshCube expects 3 arguments (width, height, length)");
            return vm->makeNil();
        }
        float width = args[0].asDouble();
        float height = args[1].asDouble();
        float length = args[2].asDouble();

        Mesh mesh = GenMeshCube(width, height, length);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        return vm->makePointer(ptr);
    }

    static Value native_GenMeshSphere(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshSphere expects 3 arguments (radius, rings, slices)");
            return vm->makeNil();
        }
        float radius = args[0].asDouble();
        int rings = args[1].asNumber();
        int slices = args[2].asNumber();

        Mesh mesh = GenMeshSphere(radius, rings, slices);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        return vm->makePointer(ptr);
    }

    static Value native_GenMeshPlane(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 4)
        {
            Error("GenMeshPlane expects 4 arguments (width, length, resX, resZ)");
            return vm->makeNil();
        }
        float width = args[0].asDouble();
        float length = args[1].asDouble();
        int resX = args[2].asNumber();
        int resZ = args[3].asNumber();

        Mesh mesh = GenMeshPlane(width, length, resX, resZ);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        return vm->makePointer(ptr);
    }

    static Value native_GenMeshCylinder(Interpreter *vm, int argc, Value *args)
    {
        if (argc != 3)
        {
            Error("GenMeshCylinder expects 3 arguments (radius, height, slices)");
            return vm->makeNil();
        }
        float radius = args[0].asDouble();
        float height = args[1].asDouble();
        int slices = args[2].asNumber();

        Mesh mesh = GenMeshCylinder(radius, height, slices);
        Model model = LoadModelFromMesh(mesh);
        Model *ptr = new Model(model);
        return vm->makePointer(ptr);
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
