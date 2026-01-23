#pragma once

#include "interpreter.hpp"
#include <raylib.h>

namespace RaylibBindings
{
    // ========================================
    // HELPER FUNCTIONS
    // ========================================

    inline float BuClamp(float value, float min, float max)
    {
        return value < min ? min : (value > max ? max : value);
    }

    inline float BuLerp(float start, float end, float amount)
    {
        return start + amount * (end - start);
    }

    // ========================================
    // MODULE REGISTRATION (called by registerAll)
    // ========================================

    void register_structs(Interpreter &vm);
    void register_core(ModuleBuilder &mod);
    void register_shapes(ModuleBuilder &mod);
    void register_textures(ModuleBuilder &mod);
    void register_text(ModuleBuilder &mod);
    void register_input(ModuleBuilder &mod);
    void register_audio(ModuleBuilder &mod);
    void register_camera(ModuleBuilder &mod);
    void register_models(ModuleBuilder &mod);

    // ========================================
    // MAIN REGISTRATION
    // ========================================

    void registerAll(Interpreter &vm);

} // namespace RaylibBindings
