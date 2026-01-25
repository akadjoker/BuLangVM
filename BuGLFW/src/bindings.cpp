#include "bindings.hpp"

 

namespace GLFWBindings
{
    void registerAll(Interpreter &vm)
    {
        ModuleBuilder module = vm.addModule("GLFW");

        register_core(module);
        register_window(module);
        register_input(module);
        register_monitor(module);
        register_context(module);
   
    }
}