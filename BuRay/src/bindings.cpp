#include "bindings.hpp"

namespace RaylibBindings
{

    void registerAll(Interpreter &vm)
    {
        // Register native structs (Vector2, Color, Rectangle, etc.)
        register_structs(vm);

        // Create raylib module
        ModuleBuilder module = vm.addModule("raylib");

        // Register all functions by category
        register_core(module);
        register_shapes(module);
        register_textures(module);
        register_text(module);
        register_input(module);
        register_audio(module);
        register_camera(module);
        register_models(module);
       
    }

}
