#include "plugin.hpp"
#include "bindings.hpp"

void register_ogre3d_module(Interpreter* vm)
{
    Ogre3DBindings::registerAll(*vm);
}

void cleanup_ogre3d_module()
{
    // TODO: Cleanup Ogre3D resources if needed
}

// Export the plugin
BU_DEFINE_PLUGIN("Ogre3D", "1.0.0", "BuLang", register_ogre3d_module, cleanup_ogre3d_module)
