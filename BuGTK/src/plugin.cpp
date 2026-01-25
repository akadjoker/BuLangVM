 

#include "plugin.hpp"
#include "bindings.hpp"

 
void register_gtk_module(Interpreter* vm)
{
    GTKBindings::registerAll(*vm);
}

// Plugin cleanup function
void cleanup_gtk_module()
{
 
    GTKBindings::gtkCleanAll();
}

// Export the plugin
BU_DEFINE_PLUGIN("gtk", "1.0.0", "BuLang", register_gtk_module, cleanup_gtk_module)
