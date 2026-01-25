#include "bindings.hpp"

// TODO: #include <Ogre.h>

namespace Ogre3DBindings
{
    // =============================================================
    // NATIVE FUNCTIONS
    // =============================================================

    // TODO: Add Ogre3D bindings here
    // Example:
    // Value native_ogreInit(Interpreter *vm, int argc, Value *args)
    // {
    //     return vm->makeNil();
    // }

    // =============================================================
    // REGISTRATION
    // =============================================================

    void registerAll(Interpreter &vm)
    {
        ModuleBuilder module = vm.addModule("Ogre3D");

        // TODO: Register functions and constants
        // module
        //     .addFunction("ogreInit", native_ogreInit, 0)
        //     .addInt("OGRE_SOME_CONSTANT", 123);
    }
}
