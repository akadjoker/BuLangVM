 

#include "plugin.hpp"
#include "bindings.hpp"

 
void register_glfw_module(Interpreter* vm)
{
    GLFWBindings::registerAll(*vm);
}

// Plugin cleanup function
void cleanup_glfw_module()
{
    
}

// Export the plugin
BU_DEFINE_PLUGIN("glfw", "1.0.0", "BuLang", register_glfw_module, cleanup_glfw_module)
