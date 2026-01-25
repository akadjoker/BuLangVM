#include "plugin.hpp"
#include "bindings.hpp"

void register_box2d_module(Interpreter* vm)
{
    Box2DBindings::registerAll(*vm);
}

void cleanup_box2d_module()
{
    // Box2D cleanup handled by b2WorldDestroy
}

// Export the plugin
BU_DEFINE_PLUGIN("Box2D", "1.0.0", "BuLang", register_box2d_module, cleanup_box2d_module)
