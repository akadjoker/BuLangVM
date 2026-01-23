// BuGLFW Plugin Entry Point
// This file exports the plugin interface for dynamic loading

#include "plugin.hpp"
#include "bidings.hpp"

// Plugin registration function
void register_glfw_module(Interpreter* vm)
{
    GLFWBindings::registerAll(*vm);
}

// Plugin cleanup function
void cleanup_glfw_module()
{
    // GLFW cleanup is handled by glfwTerminate in user code
}

// Export the plugin
BU_DEFINE_PLUGIN("GLFW", "1.0.0", "BuLang", register_glfw_module, cleanup_glfw_module)
