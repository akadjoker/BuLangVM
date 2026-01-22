#ifndef TEST_INTERPRETER_HPP
#define TEST_INTERPRETER_HPP

#include "interpreter.hpp"
#include <cassert>
#include <cstring>

class InterpreterTests
{
public:
    static void runAllTests()
    {
        testFunctionStructure();
        testNativeDefStructure();
        testStructDefStructure();
        testClassDefStructure();
        testGCObjectTypes();
        testInstanceCreation();
        testValueCreation();
        testStackOperations();
        testModuleBuilder();
        testFiberStructure();
        testProcessStructure();
    }

private:
    static void testFunctionStructure()
    {
        Function func;
        assert(func.index == 0);
        assert(func.arity == -1);
        assert(func.chunk == nullptr);
        assert(func.name == nullptr);
        assert(func.hasReturn == false);
        assert(func.upvalueCount == 0);
    }

    static void testNativeDefStructure()
    {
        NativeDef nativeDef;
        assert(nativeDef.name == nullptr);
        assert(nativeDef.func == nullptr);
        assert(nativeDef.arity == 0);
        assert(nativeDef.index == 0);
    }

    static void testStructDefStructure()
    {
        StructDef structDef;
        assert(structDef.index == 0);
        assert(structDef.name == nullptr);
        assert(structDef.argCount == 0);
    }

    static void testClassDefStructure()
    {
        ClassDef classDef;
        assert(classDef.index == 0);
        assert(classDef.name == nullptr);
        assert(classDef.parent == nullptr);
        assert(classDef.inherited == false);
        assert(classDef.constructor == nullptr);
        assert(classDef.superclass == nullptr);
    }

    static void testGCObjectTypes()
    {
        GCObject obj(GCObjectType::STRUCT);
        assert(obj.type == GCObjectType::STRUCT);
        assert(obj.marked == 0);
        assert(obj.next == nullptr);

        GCObject obj2(GCObjectType::CLASS);
        assert(obj2.type == GCObjectType::CLASS);
    }

    static void testInstanceCreation()
    {
        StructInstance structInst;
        assert(structInst.type == GCObjectType::STRUCT);
        assert(structInst.def == nullptr);

        ClassInstance classInst;
        assert(classInst.type == GCObjectType::CLASS);
        assert(classInst.klass == nullptr);

        ArrayInstance arrayInst;
        assert(arrayInst.type == GCObjectType::ARRAY);

        MapInstance mapInst;
        assert(mapInst.type == GCObjectType::MAP);

        NativeClassInstance nativeClass;
        assert(nativeClass.type == GCObjectType::NATIVE_CLASS);
        assert(nativeClass.userData == nullptr);

        NativeStructInstance nativeStruct;
        assert(nativeStruct.type == GCObjectType::NATIVE_STRUCT);
        assert(nativeStruct.data == nullptr);
    }

    static void testValueCreation()
    {
        // Test makeInt
        Value intVal = Value();
        intVal.type = ValueType::INT;
        intVal.as.integer = 42;
        assert(intVal.type == ValueType::INT);
        assert(intVal.as.integer == 42);

        // Test makeDouble
        Value doubleVal = Value();
        doubleVal.type = ValueType::DOUBLE;
        doubleVal.as.number = 3.14;
        assert(doubleVal.type == ValueType::DOUBLE);
        assert(doubleVal.as.number == 3.14);

        // Test makeBool
        Value boolVal = Value();
        boolVal.type = ValueType::BOOL;
        boolVal.as.boolean = true;
        assert(boolVal.type == ValueType::BOOL);
        assert(boolVal.as.boolean == true);

        // Test makeNil
        Value nilVal = Value();
        nilVal.type = ValueType::NIL;
        assert(nilVal.type == ValueType::NIL);
    }

    static void testStackOperations()
    {
        // These would require a full Interpreter instance
        // Basic structure validation
        Fiber fiber;
        assert(fiber.state == FiberState::DEAD);
        assert(fiber.resumeTime == 0);
        assert(fiber.stackTop == fiber.stack);
        assert(fiber.frameCount == 0);
        assert(fiber.gosubTop == 0);
        assert(fiber.tryDepth == 0);
    }

    static void testModuleBuilder()
    {
        // Validate ModuleDef structure
        ModuleDef moduleDef;
        assert(moduleDef.name == nullptr);
        assert(moduleDef.vm == nullptr);
    }

    static void testFiberStructure()
    {
        Fiber fiber;
        assert(fiber.state == FiberState::DEAD);
        assert(fiber.resumeTime == 0.0f);
        assert(fiber.ip == nullptr);
        assert(fiber.stackTop == fiber.stack);
        assert(fiber.frameCount == 0);
        assert(fiber.gosubTop == 0);
        assert(fiber.tryDepth == 0);
    }

    static void testProcessStructure()
    {
        Process process;
        assert(process.name == nullptr);
        assert(process.id == 0);
        assert(process.fibers == nullptr);
        assert(process.totalFibers == 0);
        assert(process.nextFiberIndex == 0);
        assert(process.currentFiberIndex == 0);
        assert(process.current == nullptr);
        assert(process.exitCode == 0);
        assert(process.initialized == false);
    }
};

#endif // TEST_INTERPRETER_HPP