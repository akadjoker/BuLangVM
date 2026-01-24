#include "plugin.hpp"
#include "bindings.hpp"

void register_imgui_module(Interpreter* vm)
{
    ImGuiBindings::registerAll(*vm);
}

void cleanup_imgui_module()
{
    // TODO: Cleanup ImGui context if needed
    // ImGui::DestroyContext();
}

// Export the plugin
BU_DEFINE_PLUGIN("ImGui", "1.0.0", "BuLang", register_imgui_module, cleanup_imgui_module)
