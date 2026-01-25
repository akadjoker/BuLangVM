 

#include "plugin.hpp"
#include "bindings.hpp"

 
void register_rlgl_module(Interpreter* vm)
{
    RLGLBindings::registerAll(*vm);
  
}

// Plugin cleanup function
void cleanup_rlgl_module()
{
 
    
}

// Export the plugin
BU_DEFINE_PLUGIN("RLGL", "1.0.0", "BuLang", register_rlgl_module, cleanup_rlgl_module)
