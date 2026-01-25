// BuRay Plugin Entry Point
// This file exports the plugin interface for dynamic loading

#include "plugin.hpp"
#include "bindings.hpp"

// Plugin registration function
void register_raylib_module(Interpreter* vm)
{
    RaylibBindings::registerAll(*vm);
}

// Plugin cleanup function
void cleanup_raylib_module()
{
    Info("Raylib cleanup");
    // Raylib cleanup is handled by CloseWindow in user code
}

// Export the plugin
BU_DEFINE_PLUGIN("raylib", "1.0.0", "BuLang", register_raylib_module, cleanup_raylib_module)
