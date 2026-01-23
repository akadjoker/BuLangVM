// BuSDL Plugin Entry Point
// This file exports the plugin interface for dynamic loading

#include "plugin.hpp"
#include "bindings.hpp"

// Plugin registration function
void register_sdl_module(Interpreter* vm)
{
    SDLBindings::registerAll(*vm);
}

// Plugin cleanup function (optional)
void cleanup_sdl_module()
{
    // SDL cleanup is handled by SDL_Quit in user code
}

// Export the plugin
BU_DEFINE_PLUGIN("SDL", "1.0.0", "BuLang", register_sdl_module, cleanup_sdl_module)
