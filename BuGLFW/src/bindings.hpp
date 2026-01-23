#pragma once

#include "interpreter.hpp"
#include <GLFW/glfw3.h>

namespace GLFWBindings
{
    void registerAll(Interpreter &vm);
    void register_core(ModuleBuilder &mod);
    void register_window(ModuleBuilder &mod);
    void register_input(ModuleBuilder &mod);
    void register_monitor(ModuleBuilder &mod);
    void register_context(ModuleBuilder &mod);
} // namespace GLFWBindings