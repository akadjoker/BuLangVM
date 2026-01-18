#pragma once

#include "interpreter.hpp"
#include <SDL2/SDL.h>

namespace SDLBindings
{ 
 void registerAll(Interpreter &vm);
 void register_core(ModuleBuilder &mod);
 void register_window(ModuleBuilder &module);
 void register_renderer(ModuleBuilder &module);

} // namespace RaylibBindings